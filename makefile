all:
	g++ aws.cpp -o aws
	g++ client.cpp -o client
aws:
	g++ aws.cpp -o aws
client:
	g++ client.cpp -o client
clean :
	rm client aws
