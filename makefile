all:
	cc aws.cpp -o aws
	cc client.cpp -o cpp
aws:
	cc aws.cpp
client:
	gcc -c client.cpp
clean :
	rm client aws
