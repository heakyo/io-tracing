def find_partition(partition_geom, label):
	for p in partition_geom.providers:
		if p.label == label:
			return p
	return None

def make_partition(partition_geom, label, uuid, size, start=-1, zero_partition=False):
	# Create the partition if it doesn't exist.
	print(1, __name__, partition_geom, label)	# PART:da2 isilon-pmp
	print(2, __name__, partition_geom.providers)	# [PART:da2:da2p2, PART:da2:da2p1]

	partition_provider = find_partition(partition_geom, label)
	print(partition_provider)

	if not partition_provider:
		partition_provider = partition_geom.add_partition(
			uuid, size, label, start=start, zero=zero_partition
		)
	return partition_provider
