# You have an input file that consists of lines of the form:
#
# 83032 18 07 0000 -99 20.4 -99 7.6 6.88
#
# The 3rd field is a month number, and the 2nd field is the day in that month.
#
# How would you find and print all the occurrences of 3 consecutive lines that have entries in
# consecutive days?
#
# EXAMPLE:
#
# Input file:
#
# 83032 18 07 0000 -99 20.4 -99 7.6 6.88
# 83032 27 08 0000 -99 30.6 -99 6.1 21
# 83037 29 06 0000 -99 20.4 -99 4.9 11.12
# 83037 30 06 0000 -99 18.7 -99 0.5 11.2
# 83037 01 07 0000 -99 33.5 -99 7.7 24
# 83037 05 07 0000 -99 23.3 -99 10.1 11.28
# 83037 17 07 0000 -99 21.1 -99 10.2 11.1
# 83037 18 07 0000 -99 20.7 -99 7.1 11.26
#
# Output:
#
# 83037 29 06 0000 -99 20.4 -99 4.9 11.12
# 83037 30 06 0000 -99 18.7 -99 0.5 11.2
# 83037 01 07 0000 -99 33.5 -99 7.7 24
#
# Explanation:
#
# The lines above are the only sequence of 3 consecutive lines that also denote consecutive days

BEGIN {
	i = 0
	split("31,28,31,30,31,30,31,31,30,31,30,31", m, ",")
}

function day_n(day, month)
{
	res = day
	for (k = 1; k < month; k++) {
		res += m[k]
	}
	return res
}

function is_consecutive()
{
	if (NR < 3) {
		return false
	}

	d1 = day_n(d[i], m[i])
	d2 = day_n(d[(i+1)%3], m[(i+1)%3])
	d3 = day_n(d[(i+2)%3], m[(i+2)%3])

	return d1+1 == d2 && d2+1 == d3
}

{
	d[i] = $2+0
	m[i] = $3+0
	l[i] = $0
	i = (i+1)%3
	if (is_consecutive()) {
		j = i
		for (c = 0; c < 3; c++) {
			print l[j]
			j = (j+1)%3
		}
		print ""
	}
}
