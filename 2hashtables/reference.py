import sys

# read words from input, one word per line
# then use a dictionary to count which word is most frequent
# but sometimes try to remove the word
# and then print the most frequent word and if there are multiple
# most frequent take the first one in alphabetical order

d = { }

i = 0

for line in sys.stdin:
	word = line.strip()
	is_present = word in d
	remove_it = i % 16 == 0

	if is_present:
		if remove_it:
			del d[word]
		else:
			count = d[word]
			d[word] = count + 1
	elif not remove_it:
		d[word] = 1
	i += 1

(count, word) = max(zip(d.values(), d.keys()))

for k in d:
	if d[k] == count and k < word:
		word = k

print(word, count)
