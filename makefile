all : judge fdiff 
judge : judge.cpp
	g++ judge.cpp -o judge
fdiff : fdiff.cpp
	g++ fdiff.cpp -o fdiff
