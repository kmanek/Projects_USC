all: 
	g++ -o h1 healthcenterserver.cpp -lsocket -lnsl -lresolv
	g++ -o patient1 patient1.cpp -lsocket -lnsl -lresolv
	g++ -o patient2 patient2.cpp -lsocket -lnsl -lresolv
	g++ -o doc1 doctor1.cpp -lsocket -lnsl -lresolv
	g++ -o doc2 doctor2.cpp -lsocket -lnsl -lresolv