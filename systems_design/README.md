# Systems Design

This section contains large systems design questions. System design questions may seem intimidating, but with practice and a consistent, structured approach, they can be tackled with confidence.

Some interesting resources to learn about system design, scalability, and large systems in general include:

* http://www.hiredintech.com/system-design/ (be sure to check the premium content - it's free, and it thoroughly goes through how one might design a simplified Twitter)
* http://highscalability.com/ (a very interesting blog about large scale systems)
* Cracking the Coding Interview, 6th Edition, Chapter 9 (the systems design section was significantly improved on the 6th Edition, but the 5th Edition also has some interesting ideas on Chapter 10, *Scalability and Memory Limits*)
* [Distributed Systems for Fun and Profit](http://book.mixu.net/distsys/)
* [Scalable Internet Architectures](https://www.youtube.com/watch?v=2WuT2rdLK5A)
* [The System Design interview](https://github.com/checkcheckzz/system-design-interview)
* [Thought process for System Design](http://programmers.stackexchange.com/questions/196185/general-thought-process-for-how-would-you-build-this-website-app-interview-que)
* [How would you implement Google Search?](http://programmers.stackexchange.com/questions/38324/how-would-you-implement-google-search)
* [How do I design an arbitrary system in an interview?](http://programmers.stackexchange.com/questions/75269/how-do-i-design-an-arbitrary-system-in-an-interview)
* [How to ace a Systems Design interview](https://www.palantir.com/2011/10/how-to-rock-a-systems-design-interview/)
* [Preparing System Design questions](http://www.quora.com/How-should-I-prepare-system-design-questions-for-Google-Facebook-Interview)
* [The Google Architecture (original paper)](http://infolab.stanford.edu/~backrub/google.html)

This series of lectures is a great introduction to very important concepts that underly Google's scale, such as MapReduce, GFS, BigTable, and other topics (note that GFS is old hat now, but it has historical importance. The successor of GFS is Colossus):
* [Cluster Computing and MapReduce Lecture 1](https://www.youtube.com/watch?v=yjPBkvYh-ss)
* [Cluster Computing and MapReduce Lecture 2](https://www.youtube.com/watch?v=-vD6PUdf3Js)
* [Cluster Computing and MapReduce Lecture 3](https://www.youtube.com/watch?v=5Eib_H_zCEY)
* [Cluster Computing and MapReduce Lecture 4](https://www.youtube.com/watch?v=1ZDybXl212Q)
* [Cluster Computing and MapReduce Lecture 5](https://www.youtube.com/watch?v=BT-piFBP4fE)

These talks discuss the specific issues of scalability that arised in building the systems that we all use today (Dropbox, Google, Facebook, etc.):

* [Building Software Systems At Google and Lessons Learned](https://www.youtube.com/watch?v=modXC5IWTJI)
* [How We've Scaled Dropbox](https://www.youtube.com/watch?v=PE4gwstWhmc)
* [Seattle Conference on Scalability: YouTube Scalability](https://www.youtube.com/watch?v=ZW5_eEKEC28)
* [Seattle Conference on Scalability: Lessons In Building Scalable Systems](https://www.youtube.com/watch?v=mS48X9oEar0)
* [Building Software at Google Scale Tech Talk](https://www.youtube.com/watch?v=2qv3fcXW1mg)
* [The Architecture of StackOverflow](https://www.youtube.com/watch?v=t6kM2EM6so4)
* [Facebook and memcached](https://www.youtube.com/watch?v=UH7wkvcf0ys)

Google SREs have an interesting talk about App Engine and some lessons learned:

* [Google I/O 2011: Life in App Engine Production](https://www.youtube.com/watch?v=rgQm1KEIIuc)

Some important metrics to keep in mind:

* Active users on Facebook per month: 1.45 billion
* Facebook Messenger messages/day: 800 million
* Facebook Messenger active channels peak: 7 million
* Facebook Messenger inbound traffic/sec.: 1 GB
* Skype users: 350 million
* Skype users online on average: 2.5 million
* Webpages indexed by Google: 30 trillion
* Search queries in Google per second: 40,000
* Tweets per second: 6,000

Note that there isn't a right way to answer these questions. Two different people may come up with completely divergent designs if they made different assumptions.

Like any other problem, it is important to solve design questions using a mechanical, structured approach. It is very easy to get intimidaded by questions such as "Design Google Search", or "Design TinyURL". It's scary because many brain cells from many different engineers have been devoted to the task of designing and implementing such big systems, and it took *years* and quite a few millions of dollars to get to what we have today.

Below is the approach suggested in *Cracking the Coding Interview* (6th Edition), with a few additional notes that I personally find convenient and enlightning. The process entails 5 steps:

## Step 1: Scope the Problem

The first step is, of course, to know what you are designing. Know what you're solving. Ask questions. CTCI v6 mentions designing a bit.ly service as an example. One good method that I find useful is to **imagine yourself using the system**, and at the same time imagine how you'd go about implementing the features that you, as a user, are imagining.

So, if we're designing something like bit.ly, possible questions include: how short should the shortened URLs be? Should all of them have the same size? How do we build a short URL out of a long URL? How do we find the corresponding long URL given a short URL? Do short URLs ever expire? Can a user customize a short URL? Are analytics required (they usually are in big systems for business needs)? Do short URLs expire, or do they live forever? Can a user voluntarily remove a short URL? Is there a limit on the number of URLs added by a user?

## Step 2: Make reasonable assumptions

In the context of step 1, we start making assumptions to get a feeling for how big our system will be. In his talk in Stanford - [Building Software Systems At Google and Lessons Learned](https://www.youtube.com/watch?v=modXC5IWTJI) - which is well worth watching, Jeff Dean makes it very clear that one of the most important skills to have is to estimate the cost of a design with back of the envelope calculations, so that we can evaluate and compare different designs without implementing them, which would be a huge pain.

In this step, we start going in this direction:

* Estimate the amount of data that we will have to store
* What is the peak traffic?
* How many requests / second are we expecting during peak periods?
* What is the read to write ratio? This is very important: systems with high read to write ratio benefit a lot from caching
* How do we expect the data size to grow over time?
* How many users are we expecting?

## Step 3: Design the Major components

With the previous metrics in mind, we can start designing the system. Another take home message from Jeff Dean's talk is that complex systems should always be broken down into separate services. If you're designing a search engine, perhaps it would be wise to split it into an images search service, text search, news, etc.

In this step, we shouldn't really be worried about scalability yet, we're just looking to define the major components of the system and how they will interact. Assume that you have a super powerful server with infinite memory that scales indefinitely.

Walk through an example to illustrate the flow of data in the system. See it in action. What happens when a new user request arrives?

## Step 4: Identify Key Issues and Bottlenecks

Try to answer these questions:

* What are the bottlenecks?
* What are the major issues?
* What is the first thing that is going to break when we get popular?
* What happens with peak usage?

## Step 5: Scale

This is the final step, where we take into account the results of step 4, combine them with our estimates from step 2, and try to come up with a good solution that (hopefully) scales horizontally. There are quite a number of techniques that can be used to scale a system; this only comes with practice and reading. There's no perfect solution, it is very context- and problem-dependent.

Useful resources:
* [Building Scalable Databases: Pros and Cons of Various Database Sharding Schemes](http://www.25hoursaday.com/weblog/2009/01/16/BuildingScalableDatabasesProsAndConsOfVariousDatabaseShardingSchemes.aspx)
