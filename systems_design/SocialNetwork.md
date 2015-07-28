# Problem
How would you design the data structures for a very large social network like Facebook or LinkedIn? Describe how you would design an algorithm to show the shortest path between two people (e.g. Me -> Bob -> Susan -> Jason -> You).

## Step 1: Scope the Problem

As usual, our first step is to fully understand what exactly we are designing. Facebook and LinkedIn have different use cases: the former connects friends and family throughout the world, whereas the latter targets the business world. LinkedIn is often used as a recruiting platform, and people that connect with each other are not exactly friends or family; instead, they have a professional relationship. So, what is it that we're designing here?

For the purposes of this problem, let's assume that we are interested in something more along the lines of friends and family, so we will design a social network like Facebook. But still, Facebook has quite a lot of functionality nowadays: it expanded from being a social network into a social network with chatting services, messenger notifications to mobile devices, group chats, and a lot of other functionality.

What features should we worry about? We will focus our efforts in designing the core features of a generic social network: a user has a profile where he can share some information, upload pictures, and be friends with other people. Each user has a timeline that is used to post stuff and show to the world whatever he wants.

So, the typical use cases are:

* Add a new post to the timeline
* Upload one or more pictures
* Update / change profile information
* Become friends with another user
* Visit another user's timeline and photos

As of March 2015, Facebook reports having about 1.44 billion active users / month. Our design establishes an upper bound of 1.5 billion active users / month, although it shouldn't be hard to stretch the capacity for future growth.

## Step 2: Make Reasonable Assumptions

First, let's estimate the amount of storage needed to run the social network. People nowadays upload all sorts of stupid and weird pictures all the time, so we will assume that users upload a lot of photos every day. Photos in a social network are not necessarily high quality, so let's say each photo has at most 500 kB. This errs on the side of exaggeration; services like Facebook will usually resize photos to a few hundred kilobytes.

Assuming a timespan of 5 years during which 30% of users uploads 10 photos / day, 50% uploads 2 and 20% uploads 0.1 (1 photo every 10 days), this all adds up to 5500 petabytes of storage. With a 70% capacity model, this means we are looking at a system capable of storing 8000 petabytes of photos. This is quite a lot, but we also assumed a relatively active and large userbase to begin with. Building a system capable of hosting 8000 petabytes of photos implies an extraordinarily high investment cost, so it would be wise to start small and assume a user base of a few million users and then go from there. With a smaller userbase, we could use analytics tools to extrapolate the typical user's behavior and then try to plan from there what would happen in the long run.

What about requests / second? Somewhere, there is always someone checking on Facebook, so the service is expected to have a very high number of requests / second. If we assume that each user checks on Facebook once every two hours, we will be looking at something like 200K requests / second.

In a social network like Facebook, it is also important to note that typically there is a high read-to-write ratio; people are often curious to see what their friends have written recently, their new photos, and all of that. This means that we can probably optimize access times a lot with caching.

## Step 3: Design & Scale

We could have a huge database storing everything from user profile information, to photos and timeline posts. Instead, perhaps a better alternative is to keep things logically separated: we can have one component respnsible for storing user profile information, another respnsible for photos, and another for timeline posts. This has the advantage of making it easier to scale each component independently. For example, if we stop getting new users but existing users start uploading an unusual amount of photos, then we can easily focus our efforts on adding more horsepower to the photo storage component, while leaving profile information and timeline posts untouched. However, splitting information makes it harder and more expensive to do perform lookup operations such as finding all the photos of a user, or finding all the timeline posts of a user. We can try to mitigate this by duplicating some information across the storage system, but that would complicate the design, not to mention that it kind of misses the point: after all, if we are to do that, then maybe we would be better if we just didn't split in the first place.

As mentioned before, caching can be of great help in here. We can cache queries that are expensive to answer (like listing the photos of a user, or finding his posts in the system). This has the added benefit that very popular users that get lots of visits are very likely to be cached. In fact, Facebook is one of the greatest contributors and user of memcached, and it is known that its caching infrastructure is capable of keeping several thousands of terabytes in a distributed cache to speed up requests.

As a social network, we will be essentially delivering content (photos) to people all over the world. The core architecture consists of a set of clusters of core servers holding the data and running behind hardware loadbalancers. Data can be sharded in several different ways: we could have a set of servers responsible for a given region in the globe. This has several advantages: first, people are often friends with other people in the same region, so requests to view another user's profile or photos would be mostly local to the cluster, which is good. Second, each cluster is relatively independent, so there is not a whole lot of state and synchronization to do between clusters. Third, sharding data by region allows us to place the cluster responsible for a region close to that region, thus minimizing latency. Of course, it's not a perfect solution: it doesn't seem to be fair - some regions are probably going to have much more users than other regions, which could lead to an unbalanced distribution among the clusters. On the other hand, since clusters are relatively independent, we can easily add capacity to a cluster serving a region with heavy traffic, so this doesn't seem to be a big problem.

Social networks like Facebook also make use of another concept: points of presence. PoPs in Facebook are used in regions where it is not feasible / necessary to run a full cluster of servers housing data. Instead, PoPs maintain a set of ready-to-use, opened TCP connections to the main cluster service. These PoPs serve nearby user requests and forward them to the main cluster service using the ready-to-use, established connections. Thus the user doesn't incur the overhead of establishing a connection with a server that is far away: since that connection is already established in the PoP, the user can just establish a connection with the PoP. This technique enabled Facebook to scale inexpensively while keeping the count of core clusters more or less constant. Whereas a transatlantic TCP connection handshake can take as long as 500 ms (and this is just the time to establish the connection before actually transmitting any data), establishing a TCP connection to a nearby PoP will usually take less than 100 ms. The performance boost is considerable, especially taking into account that PoPs are always ready to communicate with the cluster and as such can avoid the 500 ms delay of establishing a connection to a far away endpoint.

How can we design the algorithm to show the shortest path between two people?

Friend relationships can be seen as a graph. Finding the shortest path between two people boils down to running a BFS in the social network graph. Note that DFS is not really a great choice here, we could be looking at lots and lots of users before branching down on the right edge. But a traditional BFS might still be too expensive: for a path of length q, if each person has O(k) friends, BFS will go through O(k^q) nodes before finding a match.

We can narrow it down by using bi-directional BFS: start from the source and from the destination at the same time, and perform BFS in both directions until they collide. On average, this reduces the runtime to O(k^(q/2) + k^(q/2)). It might seem like it's not a big deal, but it's huge: if each person has 100 friends and our endpoints are 4 hops away, then traditional BFS would look at O(100^4) nodes before finding a match. Bi-directional BFS would look at O(100^2 + 100^2). That's 1 million vs. 20,000. It makes a hell of a difference!
