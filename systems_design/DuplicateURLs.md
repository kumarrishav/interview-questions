# Problem
You have 10 billion URLs. How do you select the duplicate documents? In this case, assume "duplicate" means that the URLs are identical.

## Open-ended discussion

Finding duplicate data based on a key (the URL) is usually achieved with sorting. If we sort the data and write the result to a file, the duplicates will be next to each other. However, this might not be feasible in this case for a number of reasons.

First off, how much space will 10 billion URLs take? If each URL uses 100 bytes, we have 10^12 bytes, which is roughly 1 TB. We can't sort in memory, so we will have to run an external sort algorithm. This won't be too complicated though: a commodity machine today is probably able to handle at least 512 GB of information on disk, so we can split the data in two equal halves, run external merge sort on each machine, and then make a final merge step (over the network) to write the final results to a file (possibly distributed among 2 machines too). However, sorting 512 GB of data and then merging it with another 512 GB is still a considerable computation.

Another possibility to perform sorting is to use MapReduce. Google's implementation of MapReduce makes it very easy to sort huge amounts of data because intermediate key value pairs are handed to the reducers in order, so the reducers just output the intermediate keys (map and reduce will basically be the identity functions). But using a beast like MapReduce to do this seems like overkill.

What if we hash URLs and attempt to sort based on the hashes? We know that there are 10 billion URLs, so at most there will be 10 billion unique URLs. So we need a hash function that converts a textual URL into an integer in the interval [1 .. 10 billion] (or [0 .. 9,999,999,999], that's irrelevant, we just need to hash the URLs into an interval of size 10 billion). If we do that, we can sort pretty efficiently in linear time with the help of a bitvector: if every hashed URL is a number in [1 .. 10 billion], we need a bitvector of 10 billion bits, which is roughly 10*2^30/2^3 = 1280 MB, or 1.28 GB. This fits in memory in a single machine!

So we do as follows: we create a bitvector of 10 billion bits, consuming about 1.28 GB of memory. We read the input file and hash the URL into a known integer in the interval [1 .. 10 billion]; let that integer be N. We check if N already belongs to the bitvector. If it does, we report the URL as a duplicate. If it doesn't, we add it to the bitvector.

Of course, this assumes that we have a hash function that is deterministic and collision-free. But in the context of this specific problem, we can achieve collision-free hashing, because it is one of those rare cases where the hashed keys space is at least as large as the hash input key space.
