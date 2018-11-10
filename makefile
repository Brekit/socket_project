all:
	g++ aws.cpp -o aws
	g++ client.cpp -o client
	g++ serverA.cpp -o serverA
	g++ serverB.cpp -o serverB
	g++ serverC.cpp -o serverC
aws:
	g++ aws.cpp -o aws
client:
	g++ client.cpp -o client
server:
	g++ serverA.cpp -o serverA
	g++ serverB.cpp -o serverB
	g++ serverC.cpp -o serverC
clean :
	rm client aws serverA serverB serverC
