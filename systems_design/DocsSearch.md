# Problem
Given a list of millions of documents, how would you find all documens that contain a list of words? The words can appear in any order, but they must be complete words. That is, `book` does not match `bookkeeper`.

Note: this is actually the example problem in the System Design and Scalability chapter of the 6th Edition of Cracking the Coding Interview. Here, I try to dig deeper in the problem, considering different designs and showing a few examples of doing back of the envelope calculations.

## Step 1: Scope the Problem

There is not a lot of room for ambiguity in the question - this is actually a very specific, contextualized design question. It's not vague like "design twitter".

We have one use case:

* `find_docs(w)`, where `w` is a list of words. `find_docs()` returns the list of documents that contain every word in `w`.

## Step 2: Make reasonable assumptions

We were told that we have millions of documents. But how many? It could be 2 million, or it could be 999 million. Let's take a guess here and start with 100 million.

We will also assume that documents are identified by a unique alphanumeric ID, and that each document describes the contents of a webpage. Webpages typically do not grow very much in height; they tend to be relatively short because users follow links to other pages. So let's say that each document holds the equivalent of 5 A4 pages (in practice, it is usually much less). An A4 page has something like 30 lines of text, and each line can be as long as 20 words.

So, we have 3000 words/document, summing up to a total of 300 billion words spread across 100 million documents.

If we assume that each word has 5 or 6 letters - let's err on the safe side and assume 6 - and that each letter takes up 1 byte, then each word occupies 6 bytes, so we will need roughly 6 x 300 billion = 1.8 TB just to store every word in every document.

Assuming that document IDs are case-sensitive alphanumeric strings, to index 100 million documents we need IDs with log_62(10^8) characters, which is about 27/6 = 4.5. We use logarithm of base 62 because we count the 26 letters in the range `[a-z]`, plus the 26 letters in `[A-Z]`, plus the 10 digits in `[0-9]`. log_62(10^8) can be estimated as follows: first, we convert it into base-2 logarithms (you must be familiar with powers of 2, often a reasonable assumption for someone in CS), so log_62(10^8) = log_2(10^8)/log_2(62). 10^8 is about 100 MB (because 10^6 is roughly 2^20, which is 1 MB), so in powers of 2 it is roughly 2^27 = 128 MB. 62 can be approximated by 2^6 = 64. So log_2(10^8)/log_2(62) is not very far from log_2(2^27)/log_2(2^6) = 27/6 = 4.5. Knowing how to do this is very important.

So, to index 100 million documents with alphanumeric IDs we will need 5 bytes. We can plan ahead for future growth and use 6 bytes - that will allow us to index 62^6 documents (about 2^36 documents - around 64 billion - what a difference a byte can make!).

Thus, the total storage needed to hold every document ID with its corresponding contents is 1.8TB + 6*10^8 bytes = 1.86 TB (the documents IDs don't really have an impact)

Since we want to list documents where a given set of words appear, another possibility is to store each unique word and map it to a list of documents where it occurs.

We have 300 billion words total, and using the hash table is as if each word is associated to a document ID of 6 bytes, so the total aggregate storage usage for the hash table entries is again bounded by 1.8 TB. Note that it will actually be less than 1.8 TB, because a word that occurs multiple times in the same document will only contribute with one document ID.

We need to count space to store each unique word (the keys) in the hash table. The Oxford English dictionary lists about 200,000 unique English words, but we can go wild here and use an upper bound of 400,000 unique words - these documents come from various sources where numeric sequences may also be considered as words - but either way, the difference between 200,000 unique entries and 400,000 is probably negligible in a key-value store.

A hash table with 400,000 keys of 6 bytes will need 2.4 MB to store the keys - the impact is not relevant at all.

## Step 3: Start designing

With the above metrics in mind, we can start designing our system without worrying about scalability (yet). We can have a requests server to process a query that is responsible for getting the necessary information out of the hash table and iteratively computing the result. The hash table is stored in another server that we will call the DocsServer.

To process a request, the requests server iterates through the list of words, retrieving the list of documents where a word is present from the DocsServer for each word. On each step, the results returned by the DocsServer are intersected with the current result (which is initialized to the set of document IDs where the first word is found). This keeps happening until the requests server has processed every word in the list. The final set of documents (the documents that "survived" every intersection) is the result.

## Step 4: Identify Key Issues

The most obvious issue with this design is the growth of the hash table. Our back of the envelope calculations show that we need roughly 1.8024 TB to store the hash table. This may be feasible today with a single machine, but a couple of years ago a 1 TB harddrive was rare and expensive, yet alone 2 or 3 TB harddrives. We also need to think about the future.

Undoubtdely, we will have to split the hash table into multiple servers. The question arises: how to split it? How can the requests server locate the server responsible for a given word? How do we ensure a fair and balanced partitioning?

One possibility is to split based on domain-specific knowledge. Imagine our hash table has grown to 3 TB, and we have commodity servers with 512 GB harddrives. Further assume that we can use 70% of each server's harddrive for the purposes of our system (the remaining 30% are reserved for OS, other software, etc.), so each machine has about 360 usable GB. Then we would need 3TB / 360 GB servers - about 10. This assumes a somewhat uniform, almost perfect partitioning scheme.

If our alphabet has 62 characters and we have 10 servers, we could have server1 responsible for words starting with a letter in the range `[a-f]`, server2 responsible for words starting with `[g-l]`, etc. In theory, this looks like a reasonably uniform and fair partitioning strategy, but it's not, because words in English are not uniformly distributed like this. We could end up with one or 2 servers extremely overloaded while the others could be almost empty. We can try to fix it by further splitting overloaded servers. It the server responsible for the range `[a-f]` is getting overloaded, we can have it splitting the range into `[a-c]` and `[c-f]`, using an additional server for that. We can keep partitioning like that until servers are no longer overloaded. This could be transparent for the requests server: requests for `[a-f]` still go to the same server, but then that server is smart enough to figure out if it can answer the request by itself or if it needs to consult its other friend.

We could even use idle servers (or servers that are responsible for other ranges that are almost empty) to support the other overloaded servers in tough times. For example, server5 could be responsible for the range `[t-x]` and also help server 1 with the subrange `[c-f]`. This maximizes our use of the servers (because we don't need to buy new servers to help an overloaded range while other ranges are empty), but it comes at the cost of further complications: what do we do if suddenly server5 starts getting overloaded on its own original range? We may have to back off and stop helping server1, and then server1 will be super overloaded again. Also, this shifting back and forth of keys and document IDs is expensive.

Another approach is to build an overlay network with the servers: arrange our 10 machines in a logical circular ring, where each node in the ring has a reference to the next node. Distribute the documents in a round robin fashion across the ring. The requests server keeps a reference to any valid node inside the ring, and sends the full request there. The receiving node processes the request, using its hash table to initialize the result set accordingly. Then it passes the request, along with the current result, to the next server in the ring. The next server consults its hash table and updates the result; then move the request to the next server. This happens until the request wraps around and meets the first server again, at which point the result is returned to the client.

Adding new servers or deleting existing servers is easy because we can just redistribute their keys between the remaining servers. We don't really know (and don't care) where a given entry of the hash table is stored, since requests travel across the whole ring. It solves the problem of uniform distribution reasonably well, but there is a catch: now, each server must iterate through the input words list. If we have N servers and a list of size M, we will spend O(NM) time reading the list. Furthermore, the list must be passed across the ring - this takes time and bandwidth. Any request traverses through all of the N servers, and there are O(N) network messages exchanged. Although this solves the distribution problem, it doesn't seem to scale particularly well.

Finally, whatever design we choose, it is important to mention that this seems to be a system with a very high read-to-write ratio, and as such it could benefit tremendously from caching.

## Step 5: Scale

DNS is a great example of how to successfully build a distributed databse over the Internet. DNS maps hostnames to IPs; our system maps words to lists of document IDs. Given the great success of DNS, it is safe to assume that a similar system would probably scale well - our approach to split the words into known ranges and further split overloaded ranges seems like a reasonable and scalable choice. Combining it with a front-end cache sitting between the requests server and the document servers would boost the performance quite a bit, given the system's high read-to-write ratio.
