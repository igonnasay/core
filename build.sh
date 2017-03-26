source 'config.sh'
echo
echo 'Building libcore.a...'
make libcore.a
echo 'Building libcore.a successfully.'

echo
echo 'Building api.so...'
make api.so
echo 'Building api.so successfully.'
echo

make start

