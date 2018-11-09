all:
	g++ aws.cpp -o aws
	g++ client.cpp -o client
	g++ serverA.cpp -o serverA
aws:
	g++ aws.cpp -o aws
client:
	g++ client.cpp -o client
server:
	g++ serverA.cpp -o serverA
clean :
	rm client aws serverA
