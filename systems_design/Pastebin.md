# Problem
Design a system like Pastebin, where a user can enter a piece of text and get a randomly generated URL to access it.

## Step 1: Use Cases

There are two immediate use cases that come to mind when we think about a pastebin service:

* Add a new paste in the system (and get a unique URL)
* Retrieve a paste entry given its URL

There are other possible use cases and features that we might want to support:

**Do we support text-based pastes only, or should we do something more generic like allow users to paste text and / or images?** For example, pastebin.com serves text only, and snag.gy is equivalent but it works with images. In the context of this problem, let's assume we are to design a text-only pastebin system.

**When do entries expire?** The user gets to choose it. Our service will allow a user to choose whether a paste expires within 1 hour, 1 day, 1 week, 2 weeks, 1 month or never. When an entry expires, its unique URL may be reused for new posts afterwards.

**Will the service support user accounts?** Not for now. User accounts involve a more complex system and we would have to worry about session state and a lot of other stuff, so let's assume a stateless service without user accounts. This implies that users cannot manage past entries.

**What about custom URLs?** Custom URLs are supported. The user will be given the possibility to pick a URL at his / her convenience, but this is not mandatory. However, it is reasonable (and often desirable) to impose a size limit on custom URLs, so that we have a consistent URL database.

**Is there a limit on the amount of text we can paste at a time?** Not really. The size of an entry is essentially unlimited.

**Do we support post exposure settings, like make it public, private..?** We will not worry about that for now and assume that anyone who has the URL is free to visit the paste entry.

**What about analytics? Do we need to export and keep system-wide stats?** Not for now. Analytics are useful for a number of reasons, but we will not consider them at this point.

**Conclusions**

This gives a better idea of what exactly we are building: a pastebin system where the primary use cases are adding a new paste in the system and retrieving a paste's contents given its URL. The system allows text-based pastes only. At the time of adding a new paste, users can choose whether the paste expires in 1 hour, 1 day, 1 week, 2 weeks, 1 month or never, and they can optionally pick a URL for the new post. There are no user accounts, and as such the system is mostly stateless, and users are not allowed to manage or change the settings of past entries they added. There is no limit on the size of a post, and exposure settings are not supported: anyone with access to the URL is given permission to read the corresponding post.

## Step 2: Constraints

Now we need to make some assumptions about the expected service usage and derive some constraints. Useful metrics usually include the number of expected users, the amount of storage we will need, the expected number of requests per second (even better if we can differentiate between read and write requests), how many bytes per request type (so that we can get an estimate for the overall bytes/sec. read and written), and the read to write ratio.

In this specific problem, we are not really interested in the number of users, but more on the number of posts added over time.

Now, pastebin services are not nearly as popular as Twitter or Facebook, so we can expect to have less - much less - traffic. Let's take a wild guess here and consider that 500,000 new posts are added every day. That leaves us at roughly 6 new posts per second.

What about the size of each post? This will surely vary greatly. Pastebin is used for all sorts of stuff, but a common use is for sharing source code. It is often used as a quick way to share hundreds of lines of code, or even thousands sometimes, so let's assume that each post on average contains 10 kB. At a rate of 500,000 new posts per day, that would be 5 GB/day, which adds up to a total of 18 TB over 10 years.

18 TB of posts means that we are expecting about 1.8 billion posts. Assuming a 62-letter alphabet (A-Z plus a-z plus 0-9), each post can be identified by an ID with log_62(1.8*10^9) alphanumeric characters, which is approximated by log(2^21)/log(2^6), which is roughly 5. Let's err on the safe side and use 6 letter IDs. If each alphanumeric character takes up 1 byte, and we have 1.8 billion entries, the URL IDs will eat up 10.8 GB of storage. This is not relevant when compared to 18 TB. To be safe, and to have some margin, we will assume a 70% capacity model (meaning we don't want to use more than 70% of our total storage capacity at any point), which raises our storage needs up to a little bit less than 30 TB.

We have seen before that a rate of 500K new posts per day is equivalent to 6 posts per second - this is the write rate of the system. If each post is 10 kB, the overall write ratio is 60 kB/sec. - not exactly impressive.

What about the read rate? It is pretty obvious that a pastebin service will have more reads than writes, so the read to write ratio will be greater than 1. For each new post added, how many people will click the link to see it? Again, we have to make a wild guess here, but let's say that each new post added is visited 200 times before it becomes old and people stop clicking on it. So, in other words, the read to write ratio is 200. If we have 6 write requests/sec., then with this read to write ratio we expect to have 1200 reads per second, which in bytes is equivalent to 12 MB/sec. - again, not very impressive, but it's important to keep in mind that it's a lot more than the write ratio.

Awesome! We have some specific numbers to think about during the design phase. Let's sum it up:

**Overall constraints and requirements**

* Storage (over 10 years; 70% capacity model): 30 TB (out of which 10.8 GB are used to store the posts IDs)
* Total requests/sec.: 1206
* Total read requests/sec.: 1200 (12 MB/s)
* Total write requests/sec.: 6 (60 KB/s)
* Read/write ratio: 200

## Step 3: Design

Let's ignore for a moment the amount of data we're dealing with and assume that everything fits in a single machine. How can we approach this? We will need an application service layer that processes incoming requests and forwards outbound requests. The application service layer talks to a backend datastore component. So, clients issue read and write requests to the public application service layer.

The backend datastore component acts like a big hash table of key value pairs, where the key is the post ID (6 alphanumeric characters), and the value is the text that was pasted.

This design is simple. The flow for the two use cases is described as follows:

* Add a new paste: contact the application service layer and issue a InsertPost(ID) request. The ID will be automatically generated by the application service layer if the user didn't pick one (recall that users are allowed to pick a custom ID). The application service layer forwards the request to the backend datastore, which checks to see if the requested ID is already in the database. If it is, the database is not changed and an error code is returned; otherwise, a new entry is added with the specified ID.

* Retrieve a paste entry: the flow is similar, the application service layer contacts the datastore. The datastore searches for the given ID, and if it is found, returns the post contents, otherwise, an error code is returned.

Note that in the design above, the application layer is responsible for generating an ID if the user doesn't provide one. However, the application layer has no internal knowledge of the data in the system, so it may unintentionally generate an ID that already exists. What do we do in such a scenario? We can return an error to the user, but that would only leave us in the same position and confuse the user - after all, if the user didn't pick a custom URL, how come a duplicate error was returned? It makes no sense. So when the user doesn't provide a custom URL the application layer could keep generating IDs until the datastore returns success. This has the advantage of making the datastore interface simple and relatively dumb, at the cost of a little extra complexity in the application layer. Alternatively, we could shift this responsibility to the datastore and make the application layer a dumb proxy that just forwards requests to the backend. The main advantage is that we reduce the number of interactions between the application layer and the backend datastore. If these interactions are expensive (for example, they involve remote procedure invocations), this is good. Also, the datastore layer is where all the action takes place and where all the IDs are stored, so we might as well generate IDs in there anyway.

What about post expiration? We mentioned before that the user may pick an expiration time frame. We will assume that post expiration is not an exact science; when a user picks a 1 hour expiration time frame, the post will not expire *exactly* after one hour, it may be around for a little while, but we guarantee that a post is never deleted *before* the expiration time.

We have a couple of options:

* Periodically scan all posts are purge expired entries. Say we run some automated job every hour that scans the entire dataset, reads the expiration information (we didn't really account for that piece of information, but it's negligible when compared to the size of a post) and deletes an entry if it expired. On the plus side, it's simple and it works. But there are many downsides. Scanning the dataset hourly is expensive and utterly infeasible when the data accumulates to terabytes and terabytes of information. It also introduces an hourly hiccup in the entire system, that is, every hour, there is a window of time where the system is slower and performance degrades, because we are scanning the dataset. Users notice these things. If the datastore is sharded across networked servers, this gets even worse. Also, it does sound like a brute force solution. Sure, it's simple and easy to implement, but it's probably more of a quick hack than a permanent solution.

* Keep the data sorted by expiration time. This helps because now we don't need to scan the entire dataset. We could for example store the expiration time as a 64-bit integer in UNIX time format (number of seconds since the Epoch), and keep the data set sorted according to that value. That way, the next entries to expire are always on top. We can use the same technique and periodically run a job that searches for and purges expired entries, but now we just need to iterate through the database until we hit the first entry that hasn't expired yet. This is a huge improvement, and we probably can't do much better than that, since purging N expired entries requires at least O(N) work. It does have some overhead because we need to keep the data set sorted, but there is no magical solution.

* Perform lazy expiration. This might actually be a great choice. Lazy expiration means that We Don't Care. Expired posts just sit there until some event happens and suddenly we realize that some posts expired. For example, someone makes a request to retrieve a post that expired. The datastore finds the post, but it sees that it's past due, so the post is purged and an error message is returned to the user. Or someone tried to create a new post with an ID that is already in use, but when we find that ID, we see that the associated post has expired, so we purge it and add the new post with the same ID. Or the database is being split across a new set of servers and in the process of transferring data around we find a couple of expired posts. There are many possible scenarios. The idea is that expired posts will *eventually* be purged, we don't know exactly when, and it might be a while, but we offer strong expiration guarantees to the users (in fact, stronger than the other approaches: if a request comes in for a post that expired a second ago, the server will return an error immediately, as opposed to the other approaches where a post may outlive its expiration time for a little while). This approach has the desired property of eliminating the separate cost of expiration altogether: there's no periodic job running, no "batch" purging operations, nothing. Essentially, the processing cost of post expiration is O(1). Of course, nothing in computing is free: we pay with extra storage. We may use more storage than we need and we may accumulate a considerable amount of old posts unnecessarily. However, our storage estimates did not include expired posts, we just assumed posts were added and never removed, so our storage needs remain the same.

## Step 4: Identify key bottlenecks

It is easy to see that our number one concern should be the storage layer. The first component that is likely to break in a single machine design is the database layer. In a time span of 10 years, we expect to host a considerable amount of terabytes of data. So let's focus on the database layer.

We can either use a known relational DB like MySQL, or use a non-relational model. Each choice has its advantages and drawbacks.

MySQL is a widely deployed technology across the web giants like Facebook, Google, Twitter and other similar companies. It is a mature technology that has been around for a while and polished over the course of time. Given our system's high read to write ratio, it is a good candidate for the typical master/slave replication. In this architecture, we would have a single master MySQL server taking care of writes, connected to every slave machine. Slave MySQL instances would serve read requests. Remember that we're not exactly dealing with a lot of data when it comes to reading and writing, our main concern is distributing storage. So the master/slave paradigm seems like a good fit. We do have the problem of deciding how to shard the data. A simple technique when we have N servers is to place key K in server hash(K)%N. The hash may be based on the first character of a post ID, or some other more complicated function. It can be difficult to add new servers though, we should consider how often we want to add new servers - this could be a problem.

A non-relational model does away with MySQL. We can tailor it to our specific needs, and we don't suffer the performance penalties that MySQL sometimes brings (for example, do we need all of the ACID properties of MySQL? It might be overkill). Also, our data is pretty much stateless and it doesn't have related entities, so it does look like we don't want / need a relational model anyway. We could instead use a Distributed Hash Table implementation (for example, Chord hash table), create an overlay network of servers, and go from there. This would essentially decentralize writes as well as reads, but now the application layer must be smart enough to figure out where to write to and where to read from.

In any case, there is one last component that we absolutely need: caching! With this amount of data, without caching, each read would hit the disk, which is prohibitively costly. We need to introduce a caching layer between the application layer and the backend datastore. It may be a single machine, or a small cluster of machines running memcached. For example, Facebook uses a customized version of memcached distributed across many machines to be able to cache several terabytes of data in memory. The performance gains can be huge, especially in a system like pastebin, where popular URLs are very likely to be cached.
