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
