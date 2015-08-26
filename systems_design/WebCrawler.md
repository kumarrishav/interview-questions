# Problem

Design a web crawler that downloads the Web.

## Step 1: Scope the Problem

This is not exactly a service offered to the public, but rather a more sort of private application that will be running (usually) as part of a larger system. Crawling the web is a complex task, and there are many ways to go about it. We should be asking a few questions before going any further:

* Is it a crawler for HTML pages only? Or should we fetch and store other types of media, such as sound files, images, videos, etc.?

This is important because the answer can change the design. If we are writing a general purpose crawler to download different media types, we might want to break down the parsing module into different sets of modules: one for HTML, another for images, another for videos, where each mode extracts what is considered interesting for that media type.

Let's assume for now that our crawler is going to deal with HTML only, but it should be extensible and make it easy to add support for new media types.

* What protocols are we looking at? HTTP? What about FTP links? What different protocols should our crawler handle?

For the sake of the exercise, we will assume HTTP. Again, it shouldn't be hard to extend the design to later use FTP and other protocols.

* What is the expected number of pages we will crawl? How big will the URL database become?

NetCraft estimated that there are about 1 billion websites on the Internet as of June 2014. However, a website can contain many, many URLs. Let's assume an upper bound of 15 billion different webpages that will be reached by our crawler.

* How often should the crawler update each page snapshot?

This is an important metric: together with the expected number of pages, we can get a rough approximation of how many webpages the crawler will be dealing with per second. It is also useful to estimate the amount of storage needed for a given timespan (e.g. 10 years).
We will assume that each individual page is updated every 4 weeks. A more sophisticated system might try to learn about the dynamics of each page and gradually find the optimal update time for each separate page - for example, it is probably worth keeping a news website updated daily on our crawler database, whereas a site that mirrors Linux versions doesn't need as much updates. But for now, let's set a hard, fixed cycle of 4 weeks.

## Step 2: Make reasonable assumptions

Now it is time to do some maths. If we are to crawl 15 billion pages every 4 weeks, how many pages do we need to fetch per second? That's about 3.75 x 10^9 pages per week; doing the maths brings it down to roughly 6111 pages/sec.

That's a lot of pages, a single machine is probably not going to be enough, but let's worry about that later.

What about storage? We're downloading the web, so we definitely want to keep the page's contents around. How big is a webpage? It's hard to tell, it could be as small as 1 kB, or it could be several kilobytes or even megabytes. We mentioned earlier that at this time we are worried about HTML text only, so it would be hard to be several megabytes in size. Let's settle for 100 kB - that's quite a lot of HTML text, so it looks like a reasonable assumption.

At 100 kB per webpage, we are dealing with something like 1.5 petabytes of information. We also need to store some metadata for each page - at least the URL and date of retrieval. This information can probably fit in less than 500 bytes, which would add up to 7.5 terabytes. So in total we're talking about 1.5075 petabytes of storage, which we will round up to 1.6 petabytes. Assuming a 70% capacity model (we don't want to go above 70% of the total capacity of our storage system), we will need at least 1.6 petabytes / 0.7 = 2.29 petabytes of total storage, which we will round up to 2.3 petabytes. It is also important to plan for future growth.

## Step 3: Design

A web crawler is conceptually simple to design, the major problems come with scale. The life of a web crawler basically boils down to:

* Pick the next URL to crawl from a big list of URLs on hold (often called the frontier)
* Visit the URL (this includes resolving the hostname, establishing a connection, making the HTTP request, fetch the results)
* Parse the page contents to look for new URLs; store the content
* Decide which URLs to add to the frontier, and add them
* Go back to step 1

This is a high-level view of how the system will work.

We can have a component in the architecture that is responsible for managing the frontier. This component maintains the frontier in disk, and exports methods to enqueue a new URL and dequeue the next URL. Then we need a set of what we will call *crawler servers* that are assigned with crawling tasks for a given URL. The crawler servers fetch the URL contents and store it on disk. These contents are then analyzed by another component that performs content recognition to find out if we hit a duplicate. If that's the case, we just ignore this page. Otherwise, the page is passed to another component that parses the stored pages and decides what to do with it. During that process, the parser component may, for example, copy the contents to a final location, extract the URLs, make them absolute, and pass them to the next component. The next component works as some sort of filter: it receives a URL and decides whether or not to place it in the frontier. It is important to keep the filtering procedure as a separate module because it will be easier to extend and add custom rules (for example, ignore some URLs, blacklist certain websites, etc.). It can also include some custom and clever rules to identify duplicate URLs. 

The URL frontier deserves some discussion. Simple web crawlers often do a BFS traversal of the web; this is achieved by implementing the frontier as a FIFO. However, in a massively distributed web crawler environment, performing naive BFS traversals of the web may unintentionally lead to DDoS on several websites: a website often refers many internal pages, and a BFS traversal would visit all of them in turn, possibily in parallel, which is unacceptable. A popular solution is to split the frontier into several separate queues, and use the URL's hostname to deterministically assign a URL to a given queue. Then, a limited, small number of crawler servers is responsible for each separate queue. Since only a small number of crawlers fetches URLs from each subqueue, we guarantee that web servers around the web do not drown on traffic coming from the crawler. This looks good, but one of the major drawbacks is that a popular website with lots of URLs and pages may overload the relatively small number of crawler threads responsible for that specific queue, while others may be idling because they were allotted smaller websites. Another possibility to implement the frontier is to use the AOPIC algorithm (Adaptive Online Page Importance Computation), which also has the added benefit of mitigating a common type of bot-traps (this is discussed later). Whatever we end up implementing, the URL frontier is going to be a big file. We can store it across a cluster of machines running GFS (Google File System).

Once a URL is visited and the contents are stored, it is now the job of the content recognition servers to check if this page was seen before. For that, we can keep a database of MD5 hashes of the pages seen before. If a page was seen before, we just discard it and proceed to the next crawling task. We can also perform a URL-seen test using a similar technique. Note that there is no bullet proof solution; a page with the same URL might be different each time it is loaded (because of dynamic content), or two different URLs may point to the same content (because of url shorteners, mirrors, copies of the same document). Thus it is impossible to always avoid duplicate work, but at least we minimize it.

As mentioned before, the AOPIC algorithm may be a good choice here because it minimizes the risk of the crawler getting stuck in a bot-trap. For example, consider a set of webpages that generate different URLs as part of the page contents each time they are crawled. A crawler could potentially loop infinitely and get stuck.

AOPIC solves this problem by using a credit system. Like any crawler, we start with a given set of N seed URLs (how these URLs are obtained in the first place is an entirely different matter). Each URL is originally assigned an equal amount of credits X. The system also keeps a lambda page in the frontier set; this page is not a real webpage, but rather, it's a placeholder for credit accumulation, as we will see soon. Crawling the lambda page has the side effect of redistributing all of the accumulated credits evenly among all of the other pages.

The frontier logic is as follows: when a dequeue request comes in, we pick the URL with the highest credit. Ties are broken randomly. Let Y be the credits of the selected URL. We start by charging a 10% tax of credits that is transferred to the lambda page (so, the lambda page gets 0.1Y more credits). Then we count the number of URLs in the selected page, consider there are i such URLs. Each URL is added to the frontier if not already there, and then we distribute the remaining 0.9Y credits evenly among the different URLs. So, each URL would get 0.9Y/i credits. Finally, we clear the credits of the selected page (this ensures that we will crawl a different page next time). The benefit of this approach is that it elegantly avoids bot-traps and it is polite to webservers. We say it avoids bot-traps because dynamically generated URLs are almost always unique on the web, so other external pages will not refer them. This means that each of these "malicious" URLs will be "leaking" credits to the lambda page, which from time to time redistributes credits among the other, better pages. Furthermore, good webpages that are referred by a lot of different webpages tend to accumulate a lot of credits, and so they will be crawled more often.

## Final Note

One of the exercises that comes as part of the [Go Tour](http://tour.golang.org/) (in fact, the last exercise) shows a sample implementation of a Web Crawler in Go (using a canned URL fetcher that returns predefined answers, but it's a good illustration nevertheless).

The exercise provides a very basic code showing a naive Web Crawler, and the challenge is to parallelize it and to make it smart enough so that it doesn't visit the same URL twice. I published my solution in [another repository](https://github.com/filipegoncalves/go-samples/blob/master/web_crawler/web_crawler.go).
