#! /usr/bin/awk -f

BEGIN {
    ncol = 0
    nrx = 0
    latency = 0
    maxnode = 0

    # Status: 0 → Sleep, 1 → Listen, 2 → Rx, 3 → Tx
}
/+Node/ {
    distance[$2] = $5
}

/+Total/ {
    npackets = $4
}

/^Packet/ {
    nrx += 1
    latency += $11
}

/Collission/ {
    ncol += 1
}

/^@.*SLEEPING$/ {
    now = substr($1, 2)
    node = $3
    if (node > maxnode)
	maxnode = node
    prev_mode = last_mode[node]
    duration = now - last_change[node]
    node_status[node, prev_mode] += duration
    last_change[node] = now
    last_mode[node] = 0
}
/^@.*LISTENING$/ {
    now = substr($1, 2)
    node = $3
    if (node > maxnode)
	maxnode = node
    prev_mode = last_mode[node]
    duration = now - last_change[node]
    node_status[node, prev_mode] += duration
    last_change[node] = now
    last_mode[node] = 1
}

/^@.*RECEIVING$/ {
    now = substr($1, 2)
    node = $3
    if (node > maxnode)
	maxnode = node
    prev_mode = last_mode[node]
    duration = now - last_change[node]
    node_status[node, prev_mode] += duration
    last_change[node] = now
    last_mode[node] = 2
}

/^@.*TRANSMITTING$/ {
    now = substr($1, 2)
    node = $3
    if (node > maxnode)
	maxnode = node
    prev_mode = last_mode[node]
    duration = now - last_change[node]
    node_status[node, prev_mode] += duration
    last_change[node] = now
    last_mode[node] = 3
}

END {
    print "Average latency: " latency/nrx
    print "Number of collisions: " ncol/npackets
    print "Received packets: " nrx

    /* Per node stats */
    for (i = 1; i <= maxnode; i++) {
	summary[0] += node_status[i, 0]
	summary[1] += node_status[i, 1]
	summary[2] += node_status[i, 2]
	summary[3] += node_status[i, 3]
	printf("%f Node: %d. Sleep: %f. Listen: %f. Rx: %f. Tx: %f\n",
	       distance[i], i, node_status[i, 0], node_status[i, 1],
	       node_status[i, 2], node_status[i, 3]);	
    }

    total = summary[0] + summary[1] + summary[2] + summary[3]		     
    print "\nSummary\n======="
    printf ("Sleeping: %.2f %%\nListening: %.2f %%\nReceiving: %.2f %%\nTransmitting: %.2f %%\nDelivered: %.2f %%\n",
	    100*summary[0]/total, 100*summary[1]/total, 100*summary[2]/total, 100*summary[3]/total, 100*nrx/npackets);

    print "Efficiency (Transmittin/Listening): " summary[3]/summary[1]
}
