<?
#
# let two instances of this script run concurrently
# 
# the first one should wait for 10 seconds in sleeping...
# while the second one should hang first in aquiring...

$sem_key = 1000;

$sem = sem_get($sem_key);

if(!$sem) die("sem_get failed\n");

echo "aquiring...\n";
sem_acquire($sem);
echo "aquired...\n";
sleep(10);
echo "sleeping...\n";
echo "releasing...\n";
sem_release($sem);
