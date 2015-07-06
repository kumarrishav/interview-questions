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

Since we want to list documents where a given set of words appear, another possibility is to store each unique word and map it to a list of documents where it occurs. The Oxford English dictionary lists about 200,000 unique English words, but we can go wild here and use an upper bound of 500,000 unique words - these documents come from various sources where numeric sequences may also be considered as words - but either way, the difference between 200,000 unique entries and 500,000 is probably negligible in a key-value store.

How many times does a word appear in the different documents? Some words, such as "the", "of", "we", "I", "a", "an", are extremely common and will probably have an occurrence rate very close to 100%. Others not so much, but the English language is not very rich in vocabulary, so we can at least count on the average word appearing in 80% of the documents.

This means that each entry in the hash table takes 6 bytes (word size) + 0.8*100 million*6, which is about 490 MB. In the worst case we have 500,000 entries, giving us 490 MB * 500,000 = 2450 * 10^11 bytes = 245 TB.


