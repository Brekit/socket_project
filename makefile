all:
	cc aws.cpp -o aws
	cc client.cpp -o client
aws:
	cc aws.cpp -o aws
client:
	cc  client.cpp -o client
clean :
	rm client aws
