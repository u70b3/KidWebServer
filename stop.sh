PIDFILE="$PWD/pid"
PID=$(cat $PIDFILE)
# kill -9 $PID
kill $PID
