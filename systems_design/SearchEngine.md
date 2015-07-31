# Problem

Design a search engine like Google Search.

## Step 1: Use Cases

A search engine has one simple use case:

* Search the Web with a set of keywords

However, the question is a little bit vague, search engines like Google have lots of features these days.

**What document types should we support?** An engine like Google Search typically allows a user to search for webpages, images, videos, maps, etc. For simplicity, let's assume that we are to design a search engine for webpage search, so we just index HTML pages. 

**Should we worry about the crawling module? Or should we just assume we somehow have already downloaded the web?** One of the components that makes a search engine run is the web crawler - in fact that's one of the most important pieces of any search engine. In this question, we will assume that web crawling is already taken care of, we just need to think about how to structure the architecture to allow efficient lookup and keyword matching on the webpages our crawler found.

**Do we need any fancy features like auto-complete, spell checker,... ?** Not at this point. These are useful features, but they are not core. Let's leave them as possible future tweaks.

## Step 2: Constraints

This is where we get to do some maths. We need some concrete numbers to know what exactly we're dealing with. The latest internet stats reports mention that Google answers an average of 40,000 search queries per second. At the system level, these are perceived as reads. What about writes? A search engine is another type of system where the read to write ratio is huge. By far, the most common operation is to read from the search index. Also, there is no clear concept of when writes happen, because users never write, that's the job of the web crawler. From time to time, each webpage in our search engine is eventually updated by the web crawler, these are the writes because we need to update the indexes. We will disregard writes for now, since it is not clear from the problem description when exactly (and how often) the pages are updated. As mentioned before, we're not worrying about the crawling module, so the assumption is that the data is just there.

Now, the average user will not want to browse through every page that matched the search keywords, so it is safe to assume that the reply to a search query is only a couple of kilobytes. After all, it's just an HTML page listing a bunch of hyperlinks to other webpages around the world - not rocket science. Even if the user wants to browse further into the result set, he will have to issue new requests to fetch the next set of results (assuming we will break the result set into reasonably sized chunks, like Google does), but these requests are already accounted for in our 40,000 reqs/sec. estimate, so this is probably a reasonable assumption.

So, we will assume that each request generates a reply of 100 kB. With 40,000 requests/sec., we are looking at something like 4 GB/s of outbound traffic (or 32 gbps in network units). Inbound traffic is considerably smaller, since search queries mostly consist of HTTP GET requests with the search keywords. For example, if a search request is 2 kB, we have 80 MB/s of inbound traffic (640 mbps).

Let's talk about storage. Webpages themselves are not usually blatantly huge, but there are lots and lots of them. Everywhere. Webpages everywhere! According to information made public by Google, there are about 30 trillion webpages out there nowadays. Now, we will not make an estimate of the space needed to actually *store* the webpages for many reasons: first, the results will be scary (haha! Oh, Google...!). Second, we were told that the web crawler has already been dealt with, and storing the webpages is sort of the web crawler's job. But the search engine needs to identify these webpages, probably with some form of an ID, and these IDs will have to be stored somewhere in an inverted index. So instead, we will estimate the storage needs for the document IDs.

We assume that a document ID is composed by alphanumeric characters in the ranges [A-Z], [a-z] and [0-9]. The IDs are case-sensitive. This leaves us with a 62-element alphabet for the ID key space. To identify 30 trillion documents, we need log_62(3\*10^13), which is more or less the same as log_2(2^5\*2^40)/log_2(2^6) = 45/6, so we only need 8 symbols per ID. Since we only use symbols in the English alphabet, it is reasonable to assume each symbol is 1 byte, so we need 240 TB to store all the document IDs. A search engine will need some sort of inverted index scheme at the very least; if each document has an average of 1,000 unique English words, then the document IDs storage goes up to 240 PB, which is *a lot*. As usual we assume a 70% capacity model; 24/7 < 4, so we would be ok with 400 PB capacity.

The Oxford English Dictionary lists between 100,000 and 200,000 English words; let's err on the safe side and place an upper bound of 500,000 unique words. Note that this assumes a perfect world where English is the only true and one language. In practice, Google indexes pages all over the world, so the word count estimate is clearly off the hook. This is not necessarily bad, we just have to be aware that our assumption is not realistic if we scale to other countries. If a word in English takes an average of 32 bytes, 500,000 words take up 16 MB. Not a big deal.

Ok, we now have some concrete numbers that make this problem less abstract. Here's what we know so far:

* Storage requirements (70% capacity model): 400 PB (assumes 30 trillion unique webpages)
* Requests / sec.: 40,000 (out of which 40,000 are reads - we disregard writes because this is intimately tied to the web crawler module)
* read() reqs. inbound traffic: 80 MB/s (640 mbps)
* read() reqs. outbound traffic: 4 GB/s (32 gbps)

## Step 3: Design

As usual, we abstract away our Big Data requirements for now and assume that one machine is enough. We define an architecture with the following building blocks:

* Application layer: this is the public face of the service. It's where clients go to in order to perform queries. The application layer does some simple input validation and forwards the search queries to the backend layer.

* Backend layer: this layer implements the search algorithm. It starts by retrieving the associated document IDs for each word with the help of an inverted index (we will describe this component later on). As results arrive, the backend layer intersects them with what it had before, so by the end of this stage we have a list of document IDs that matched every keyword. Then, it invokes another component - we will call it the URL index - to fetch the URL for each document ID in the result set. Finally, it sorts the results by some predefined rank value, generates an HTML page, and sends it back to the client.

* Inverted Index: a component that keeps a mapping from English words to Document IDs. As mentioned before, this table is expected to be as large as 240 PB to hold the document IDs, and we plan to allocate it about 400 PB in a 70% capacity model, plus 16 MB for the key space (English words).

* URL Index: this is another component that maps document IDs to URLs. The storage needs for this table were not estimated because we assume that this is part of the web crawler's architecture.

## Step 4: Identify Key Bottlenecks & Scale

The original design assumes that a client is interested in the entire results of a query. This is not often the case, so there is some room for optimization. For example, instead of fetching all the documents that match each word iteratively (which will involve a lot of data flowing between the backend and the inverted index components), we could send the full list to the inverted index module, along with an offset value, and let it compute the result set for us. This minimizes the interactions between these two components, which will likely be separated by a network when we scale the system, and it gives us the opportunity to limit the number of results returned. For example, the inverted index component could limit each result set to at most 20 items. The offset value sent by the backend indicates the offset of the first result in the results list that the client is interested in. An offset of 0 means "give me the first 20 results", an offset of 1 means "give me the next set of 20 results (after the first)", etc. This can have a tremendous impacts in the query reply latency time because transferring 20 document IDs over the network is extremely fast and lightweight, and the client is probably not interested in more than that: user experience is not affected, and the communication between the components drops by a huge factor. As a small improvement, we can also have the backend pass in the number of results desired rather than hardcoding that in the inverted index.

The downside is that now we need to keep some state across web searches. What do we do when a client asks for more results? Well, search queries are essentially stateless, and they can remain so if we just add a new field in the client request indicating the result offset (and then the backend layer just forwards this to the inverted index component). It looks like this is what Google does; fetching the next page of results changes an offset parameter that is sent from the client in the HTTP GET parameters.

Given the large traffic amount and storage requirements, we need to think about sharding. It is probably not a good idea to use a relational DB model for a number of reasons: MySQL and other database engines are not really prepared to handle 240 PB databases, not even with master/slave partitioning and similar techniques. A system that is capable of storing 400 PB of information will need several thousands of machines, the usual partitioning schemes offered by MySQL and similar tools will not scale.

The backend component is relatively easy to distribute, because it just forwards requests and replies to the clients. A static web server like Apache can usually serve around 800 requests/sec. without a hitch, so to serve 40,000 requests we need roughly 50 machines. These can sit behind one or more hardware load balancers. There are many different choices as to which load balancing method to pick, but we won't go into detail.

The inverted index is more of a challenge. Google and other indexing servers that house this amount of data use multiple levels of sharding. Exactly how many levels would depend on empirical factors, so it's hard to tell. But let's say a machine is capable of holding 2 TB of the inverted index. This means we would need 200,000 machines for the inverted index. Note that the index key space (English words) does not change over time, which makes it easier to partition. We could divide the alphabet into equally sized chunks and assign each chunk to a set of machines. Each cluster would then implement its own internal sharding scheme, according to that chunk's particular key distribution. It could repartition the chunk into smaller chunks across smaller clusters, or it could assign key value pairs round-robin, or use some distributed hash table. There's no "best" choice here, but it would be wise to see how the system grows before sticking to a particular partitioning scheme.

Last but not least, caching is always a good idea. We can introduce caching layers in pretty much every component of the architecture: the backend, the inverted index, and the URL index.

**NOTE**: See [WebCrawler](https://github.com/filipegoncalves/interview-questions/blob/master/systems_design/WebCrawler.md) for some thoughts about the design of the Web Crawler component.
