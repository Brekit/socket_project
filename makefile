all:
	g++ aws.cpp -o aws
	g++ client.cpp -o client
	g++ serverA.cpp -o serverA
	g++ serverB.cpp -o serverB
	g++ serverC.cpp -o serverC
	g++ monitor.cpp -o monitor
aws:
	g++ aws.cpp -o aws
client:
	g++ client.cpp -o client
server:
	g++ serverA.cpp -o serverA
	g++ serverB.cpp -o serverB
	g++ serverC.cpp -o serverC
monitor:
	g++ monitor.cpp -o monitor
clean :
	rm client aws serverA serverB serverC monitor
