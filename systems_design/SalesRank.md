# Problem
A large eCommerce company wishes to list the best-selling products, overall and by category. For example, one product might be the #1056th best-selling product overall but the #13th best-selling product under "Sports Equipment" and the #24th best-selling product under "Safety". Describe how you would design this system.

# Step 1: Scope the Problem

First, we need to know if there is any limit on how many best-selling products we want to list. The problem mentions an example where a product is the 1056th best-selling product overall. Is this something we are interested in? Or we just want, say, the top 20? In other words, do we want the entire product base sorted, or only a subset? This is imporant to know what kind of scale we're dealing with.

Since the problem mentions an example with the 1056th best-selling product, we will assume we want to sort everything.

So, this question is really about building an analytics system that sorts products based on the number of sales, both overall and category wise.

# Step 2: Make Reasonable Assumptions

How big is this eCommerce platform, and how is it designed? Let's say there are a few million users (say 10 million), and the average user makes one purchase every week. So we have about 10 million purchases / week, which is about 15 / second.

We assume that the traffic in the website is much larger, since people usually like to look around and buy less often. Surely, a very large eCommerce website has many more purchases per second, but let's settle for 15 purchases/sec.

Is it easy for the platform to communicate to other APIs when a purchase takes place? We are interested in this because we want to make this information available to the analytics component we are designing. For now, let's assume that the API can be modified to notify a third-party service every time a client checks out, but let's not forget about other alternatives.

And how many categories do we have? Well, not a lot - let's say about 30 categories. People don't like a lot of categories because it starts to get confusing and hard to choose (and remember) - 30 is actually quite a high number.

Another important metric is how many different products exist, and what is their distribution among different categories. Amazon sells hundreds of millions of products. Let's assume that this eCommerce platform is not as big as Amazon, but it still has quite a lot of products - about 20 million. Assuming a uniform distribution (which might not really be accurate...), this places about 666,666 products in each category.

So, we want to design a system that keeps a global sorted view on the number of sales of all of the 20 million products, and also keeps a sorted view of each category.

Finally, what is the timespan of the ranking system? It is a rank over all the sales ever done, or should we limit it to, say, last week's sales? Some analytics tools for business people need the overall data to become more and more accurate over time and to accurately predict trends and analyze the business over the years. However, this problem seems to come up in a user context, where people are just interested in seeing the recent trends. So we will assume that the rank data should include sales that took place within the last week.

# Step 3: Design

Conceptually, before thinking about the components, we have to make a decision: should we instantly update our ranking metrics when a purchase takes place? Or is it ok if we just fetch updated data periodically? Analytics / ranking systems usually don't need to be 100% exact, and in fact, that's not advisable, as the cost of doing so would be prohibitively high without any immediate benefit. It's ok if the 12th item listed should now be the 10th or the 15th, we don't need the exact metrics, an approximation is fine. We assume that the eCommerce platform is only smart enough to send a notification to some other service when a purchase takes place. But what do we do with such notification? We can:

* Immediately update the overall and category-wide ranking data, essentially providing a real-time rank service to the frontend. With an average of 15 (or more) purchases per second, and 20 million products out there, this approach seems to scale poorly as we would have to sort millions of entries many times per second. Well, the design is simple, we just need a circular database buffer big enough to store one week worth of purchases, and each time a new entry is added, we just sort it. But yes, it's extremely expensive, and it is too precise; we don't need to constantly update everything.

* Periodically update the rank data. In this design, we would have another component in our architecture that stores purchase entries that weren't processed yet. There would be a logging module where we accumulate this data, and periodically (say every 30 minutes) the ranking module queries the logging module to get the latest updates. This information is added to the ranking module, we sort everything again, and we update stats accordingly. This seems like a better choice here: as long as the logging module can handle the overall purchase/second load, the ranking system will scale naturally, because even if the number of purchases per second grows, the rate at which we perform sorting remains constant (but of course, we will get more data during the same time interval).

So, periodic updates it is. How do we design the logging system? Since the rest of the eCommerce app is already designed, and we're just interested in sorting by sales amount, the logging system only needs to keep track of the number of times each product in each category was bought. So we can have a set of hash tables - one per category, plus one for overall purchases, which can be seen as an additional category - that tells how many times a given product with a given ID was bought since the last time the ranking system fetched data from the logging system. Well, in fact, now that we mention the ranking system, we don't really need to keep a set of hash tables in the logging system - a single hash table will do. We will have to store product counting by category in the ranking system anyway, so why duplicate information in two places?

Here's the typical flow:

* A new purchase occurs
* The new purchase information (product ID) is sent to the logging system
* The logging system increments the count of purchases seen for that product
* Eventually, the ranking system will fetch this updated information from the logging system
* When new information is fetched by the ranking system, the logging system is cleared, because we won't fetch it again. So the logging system acts as a temporary buffer
* The ranking system, in turn, merges this new information into the database and sorts everything. Also, it purges data that is older than one week

Let's assume a product ID is 8 bytes long, and the associated count number is 4 bytes (so 12 bytes per entry). In the worst case, there are 20 million entries in the logging system on hold, so that would be about 240 MB. Not a big deal - we can even keep it in memory! A simple hash table will do. Also, it is relatively scalable; if the hash table grows as large as 1 GB, that would mean we have about 0.84*10^8 = 84 million products on hold, so we have some growth margin. Remember that the hash table is cleared when the ranking system fetches new information, which happens every 30 minutes (we could actually tune this value over time).

Ok, now for the ranking system: first, it needs to keep the count of purchases for each known product in the last week. We could just have a large list of (product ID, count) pairs, but this would make it hard to update the counts over the course of a week, because we wouldn't know how to update the count to disregard the purchases that happened in the last week. We need to know how many times each product was bought each day within the time frame of 1 week. So the list is a little bit more complicated: for each product ID, we have 7 columns (Monday through Sunday) that count the purchases on each day, and we can have an 8th column with the total for the week. Then it is trivial to update each entry in the next day: for example, to update Tuesday, subtract Tuesday's count from the total, update it to the new value, and add that value to the total. Done. Note that the ranking system must be smart and detect the first time in a day that it fetches data from the logging system. That's when the day's value is cleared; subsequent requests simply add the new data to that day's already known value.

How much data will the ranking system hold? In the worst case, all 20 million products are part of the overall ranking, but now each entry has 8 columns of 4 bytes each (plus the 8 bytes key), so the overall ranking uses about 800 MB of space. Note that this is just for the overall ranking, we didn't count categories in here.

Once we sort the global category and every other category, we write the results into a bunch of files and make them available to the frontend components.

# Step 4: Identify Key Issues

The logging system doesn't seem to be the bottleneck here; we saw earlier that we can easily accommodate a resonable product-base growth. The ranking database is probably what will break first. The exact numbers depend on how products are distributed across categories, but if the average product appears in 30% of the categories (so each product appears in 9 categories), and recalling that each entry takes up 40 bytes, then we'd need 180 million * 40 bytes = 7200 million bytes = 7.2 GB. If we account for the overall ranking (800 MB), then we expect to use about 8 GB of storage to keep the ranking system running. It starts to be a little too much to fit in memory (although it is certainly feasible in today's hardware), but it is not exactly large scale. In any case, we wouldn't sort everything anyway, so we can just keep the ranking for each category in a different file and sort each one in turn.

Note that if we assume that the products are only in 1 category, then we could sort each category to get the rankings by category and then we could perform an N-way merge to get the overall rankings. But we assumed that a product may belong to many categories (as is conveyed by the problem statement), so we can't do this.
