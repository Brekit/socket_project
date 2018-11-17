all:
	g++ aws.cpp -o awsoutput
	g++ client.cpp -o client
	g++ serverA.cpp -o serverAoutput
	g++ serverB.cpp -o serverBoutput
	g++ serverC.cpp -o serverCoutput
	g++ monitor.cpp -o monitoroutput
aws:
	./awsoutput
client:
	./client
serverA:
	./serverAoutput
serverB:
	./serverBoutput
serverC:
	./serverCoutput
monitor:
	./monitoroutput
clean:
	rm client awsoutput serverAoutput serverBoutput serverCoutput monitoroutput
