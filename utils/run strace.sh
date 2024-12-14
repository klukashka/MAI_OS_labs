gcc first.c -o first -lm
strace ./first
rm first

gcc second.c -o second -lm
strace ./second
rm second