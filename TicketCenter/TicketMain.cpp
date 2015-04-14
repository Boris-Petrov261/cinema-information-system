#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <random>
#include <algorithm>
#include <cstdio>
#include "TicketCenterHeader.h"
using namespace std;

//contains info about all events and their dates
ofstream database_Of("database.txt", ios::app);
ifstream database_If;
//keeps track of how many events there are
ofstream eventsCount_Of("events_count.txt", ios::app);
ifstream eventsCount_If;
//contain the unique codes of all bought tickets
ofstream validTickets_Of("valid_tickets.txt", ios::app);
ifstream validTickets_If;
//keeps track of the seats available for each event
ofstream seatsAvailable_Of("seats_available.txt", ios::app);
ifstream seatsAvailable_If;
//all information about seats that are booked but not yet bought
ofstream bookedTickets_Of("booked_tickets.txt", ios::app);
ifstream bookedTickets_If;
//how many tickets were bought for each event
ofstream statistics_Of("statistics.txt", ios::app);
ifstream statistics_If;

const int MAX_EVENTS = 50; // max number of events
const int MAX_VIEWERS = 10; // for each row/column 
short seats[MAX_EVENTS][MAX_VIEWERS][MAX_VIEWERS] = {0}; // available seats for each event as nxn matrix  
int boughtTickets[MAX_EVENTS] = {0}; //for statistics

//random numbers c++11 standard
std::random_device rd;     // only used once to initialize engine
std::mt19937 rng(rd());      // random-number engine used
std::uniform_int_distribution<> uni(65,90); // guaranteed unbiased

//checks whether the file is empty / is created for the first time
bool is_file_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

/* no use for this function keep it just in case
std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}*/

void getLineNumber(std::ifstream& file, int num, std::string& line)
{
	int initialLine =1;
	if(num>1){
		while(getline(file, line)&& num!=initialLine)
			initialLine++;
	}else
		getline(file,line);
}

//count the number of lines in a text file
int lineCount(std::ifstream& file)
{
	int counter = 1;
	string line;
	while(getline(file, line))
		counter++;
	return counter-1; // don't count the last line because it's just empty space
}

void newEvent()
{
	string eventName, date;
	cout << "Enter event name: ";
	getline(cin,eventName);
	int n =0;
	int day=0, month=0, year=0;
	do{
	cin.sync();
	cout << "Enter date of event(dd/mm/yyyy): ";
	
	//get the date as integers seperated by slashes / 
	getline(cin,date);
	n = sscanf_s(date.c_str(), "%i/%i/%i", &day, &month, &year); 
	if (n!=3||day <1||day >31 || month<1 || month >12) 
		cout << date << " is not a valid date." << endl;
	}while(n!=3||day <1||day >31 || month<1 || month >12);
	cout << "\"" << eventName << "\" set to air on " << date << endl;
	

	string line;
	int numOfEvents = 0;
	if(is_file_empty(database_If))
		numOfEvents = 1;
	else{
		getline(eventsCount_If, line);
		numOfEvents = (line[0] - '0') + 1;
	}
	//increase the number of events by one
	eventsCount_Of.close();
	eventsCount_Of.open("events_count.txt" , ios::trunc);
	eventsCount_Of << numOfEvents << endl;
	//add the event to statistics
	statistics_Of << eventName << " - 0" << endl; 
	database_Of << numOfEvents << ". " <<eventName << " *** " << date <<endl;

	//save changes
	database_Of.flush();
	eventsCount_Of.flush();
	statistics_Of.flush();

	//wait for user to press enter
	system("pause");
	showMainScreen();
}

void showAvailableSeats()
{
	int inputEvent;
	do{
		cout << "Choose an event: " << endl;
		printEvents();
		cin >> inputEvent;
	}while(inputEvent <1||inputEvent >getCountOfEvents());
	cin.sync();

	string line;
	cout << "Available seats(0-available, 1-taken, 2-reserved):" << endl;
	getLineNumber(seatsAvailable_If, inputEvent, line);
	for(int i =0; i < MAX_VIEWERS; i++)
	{
		for(int j =0; j < MAX_VIEWERS; j++)
			cout << line[i*20 + j*2] << " ";
		cout << endl;
	}

	system("pause");
	showMainScreen();
}

void bookTicket()
{
	int inputEvent;
	do{
		cout << "Choose an event: " << endl;
		printEvents();
		cin >> inputEvent;
	}while(inputEvent <1||inputEvent >getCountOfEvents());
	cin.sync();

	string comment;
	int row=0, column=0;
	do{
	cout << "Choose row: "; cin >> row;
	cout << "Choose column: "; cin >> column;
	cin.sync();
	if(row<1 ||row>MAX_VIEWERS || column <1 ||column >MAX_VIEWERS)
		cout << "Row and column must be <" << MAX_VIEWERS<< endl;
	}while(row<1 ||row>MAX_VIEWERS || column <1 ||column >MAX_VIEWERS);
	cout << "Add a comment: "; getline(cin, comment);

	string eventName;
	database_If.close();
	database_If.open("database.txt");
	getLineNumber(database_If, inputEvent, eventName);

	bookedTickets_Of << eventName.substr(3) << " Row " << row << " Column " << column << " " << comment << endl;
	bookedTickets_Of.flush();
	cout << "Booking complete!" << endl;
	seats[inputEvent-1][row-1][column-1] = 2;
	saveChangesToSeats();
	system("pause");
	showMainScreen();
}

void buyTicket()
{
	int inputEvent;
	do{
		cout << "Choose an event: " << endl;
		printEvents();
		cin >> inputEvent;
	}while(inputEvent <1||inputEvent >getCountOfEvents());
	cin.sync();
	
	int row= 0, column = 0;
	string ticketCode;
	do{
	cout << "Choose row: "; cin >> row;
	cout << "Choose column: "; cin >> column;
	if(seats[inputEvent-1][row-1][column-1] == 1)
		cout << "Seat is taken." << endl;
	if(row<1 ||row>MAX_VIEWERS || column <1 ||column >MAX_VIEWERS)
		cout << "Row and column must be <" << MAX_VIEWERS << endl;
	}while(seats[inputEvent-1][row-1][column-1] == 1 || row<1 ||row>MAX_VIEWERS || column <1 ||column >MAX_VIEWERS);
	for(int i = 0; i <6; i++)
	{
		//random char
		char a = uni(rd);
		ticketCode += a;
	}
	ticketCode += '&';
	if(row < 10)
		ticketCode += '0' + to_string(row);
	else
		ticketCode += to_string(row);
	if(column < 10)
		ticketCode += '0' + to_string(column);
	else
		ticketCode += to_string(column);

	seats[inputEvent-1][row-1][column-1] = 1;

	validTickets_Of << ticketCode << " ";
	validTickets_Of.flush();
	cout << "Ticket bought: " <<ticketCode;
	cout << endl;
	saveChangesToSeats();
	boughtTickets[inputEvent-1] +=1;
	updateStatistics();

	system("pause");
	showMainScreen();
}

void showBookedSeats()
{
	string line;
	int counter = 1;
	bookedTickets_If.close();
	bookedTickets_If.open("booked_tickets.txt");
	if(is_file_empty(bookedTickets_If))
	{
		cout << "No reservations yet." << endl;
		system("pause");
		showMainScreen();
		return;
	}
	while(getline(bookedTickets_If, line))
	{
		cout << counter << ". " << line << endl;
		counter++;
	}
	
}

void cancelReservation()
{
	if(is_file_empty(bookedTickets_If))
	{
		cout << "No reservations yet." << endl;
		system("pause");
		showMainScreen();
		return;
	}
	int inputEvent=0;
	do{
		cout << "Choose a reservation to cancel: " << endl;
		showBookedSeats();
		cin >> inputEvent;
		bookedTickets_If.close();
		bookedTickets_If.open("booked_tickets.txt");
	}while(inputEvent <1||inputEvent >lineCount(bookedTickets_If));
	cin.sync();

	bookedTickets_If.close();
	bookedTickets_If.open("booked_tickets.txt");
	string reservation;
	getLineNumber(bookedTickets_If, inputEvent, reservation);
	int row, column;
	row = atoi(reservation.substr(reservation.find("Row ")+4, 2).c_str());
	column = atoi(reservation.substr(reservation.find("Column ")+7, 2).c_str());
	
	string eventTitle = reservation.substr(3, reservation.find(" ***"));
	database_If.close();
	database_If.open("database.txt");
	string line;
	int counter=1;
	while(getline(database_If, line))
	{
		if(eventTitle.compare(line.substr(3, line.find(" ***"))))
			break;
		else
			counter++;
	}
	seats[counter-1][row-1][column-1] =0;
	saveChangesToSeats();

	//removes the reservation from the text file
	string temp, line2;
	int counter2 = 1;
	bookedTickets_If.close();
	bookedTickets_If.open("booked_tickets.txt");
	while(getline(bookedTickets_If, line2))
	{
		if(counter2 != inputEvent)
			temp+= line2 +"\n";
		counter2++;
	}
	bookedTickets_Of.close();
	bookedTickets_Of.open("booked_tickets.txt", ios::trunc);
	bookedTickets_Of << temp;
	bookedTickets_Of.flush();

	system("pause");
	showMainScreen();
}

void checkTicketValidity()
{
	string searchFor;
	cout << "Enter unique ticket code: ";
	getline(cin, searchFor);
	string line;
	getline(validTickets_If, line);
	if(line.find(searchFor)!=string::npos&&searchFor.length() == 11)
	{
		cout << "Ticket " << searchFor << " is valid." << endl;
		int rowNum = atoi(searchFor.substr(7, 2).c_str());
		int columnNum = atoi(searchFor.substr(9, 2).c_str());
		cout << "Row " << rowNum << " Column " << columnNum;
	}
	else
		cout << "Ticket " << searchFor << " is not valid.";
	cout << endl;

	system("pause");
	showMainScreen();
}

void showRecentEvents()
{
	string startDate, endDate;
	int day1=0, month1=0, year1=0;
	int n1=0;
	int day2=0, month2=0, year2=0;
	int n2=0;

	do{
		cin.sync();
		cout << "Enter start date(dd/mm/yyyy): ";getline(cin, startDate);
		n1 = sscanf_s(startDate.c_str(), "%i/%i/%i", &day1, &month1, &year1);
		if (n1!=3||day1 <1||day1 >31 || month1<1 || month1 >12) 
			cout << startDate << " is not a valid date" << endl;
	}while(n1!=3||day1 <1||day1 >31 || month1<1 || month1 >12);
	
	do{
		cin.sync();
		cout << "Enter end date(dd/mm/yyyy): "; getline(cin, endDate);
		n2 = sscanf_s(endDate.c_str(), "%i/%i/%i", &day2, &month2, &year2);
		if(n2!=3||day2 <1||day2 >31 || month2<1 || month2 >12)
			cout << endDate << " is not a valid date" << endl;
	}while(n2!=3||day2 <1||day2 >31 || month2<1 || month2 >12);

	cout << "All events from " << startDate << " to " << endDate << ":" << endl;
	for(int i =1; i <= getCountOfEvents(); i++)
	{
		string currentEventDate;
		getline(database_If, currentEventDate);
		int day, month,year;
		sscanf_s(currentEventDate.substr(currentEventDate.find(" ***")+4).c_str(), "%i/%i/%i", &day,&month,&year);
		if((year <= year2 && year >= year1)||((year1==year2==year)&&month <= month2 && month>=month1)||((year1==year2==year)&&(month1==month2==month)&&day <=day2 &&day>=day1))
		{
			string line;
			statistics_If.close();
			statistics_If.open("statistics.txt");
			getLineNumber(statistics_If, i, line);
			cout << i<<". " << line << " ticket(s) bought" << endl;
		}
	}

	system("pause");
	showMainScreen();
}

void showStatistics()
{
	cout << "Most popular events: " << endl;
	string eventNamesOrder[MAX_EVENTS];
	int descendingOrderBoughtTickets[MAX_EVENTS];

	//copy array
	for(int i = 0; i < getCountOfEvents(); i++)
	{
		descendingOrderBoughtTickets[i] = boughtTickets[i];
	}

	for(int i = 1; i <= getCountOfEvents(); i++)
	{
		string line;
		database_If.close();
		database_If.open("database.txt");
		getLineNumber(database_If, i,line);
		eventNamesOrder[i-1] = line.substr(3, line.find(" ***")-3);
	}
	
	//sort in descending order
	for(int i = 0; i <getCountOfEvents(); i++)
	{
		for(int j = 0; j <getCountOfEvents(); j++)
		{
			if(descendingOrderBoughtTickets[i] > descendingOrderBoughtTickets[j])
			{
				int temp = descendingOrderBoughtTickets[i];
				descendingOrderBoughtTickets[i] = descendingOrderBoughtTickets[j];
				descendingOrderBoughtTickets[j] = temp;
				string tempString = eventNamesOrder[i];
				eventNamesOrder[i] = eventNamesOrder[j];
				eventNamesOrder[j] = tempString;
			}
		}
	}

	for(int i = 1; i <= getCountOfEvents(); i++)
	{
		cout << i << "." << eventNamesOrder[i-1] << " - " << descendingOrderBoughtTickets[i-1] << " ticket(s) sold" << endl;
	}

	system("pause");
	showMainScreen();
}

void printEvents()
{
	database_If.close();
	database_If.open("database.txt");
	string line;
	if(database_If.is_open())
	{
		while(getline(database_If, line))
		{
			cout << line << endl;
		}
	}
}

void saveChangesToSeats()
{
	//save the changes of the available seats to a text file
	seatsAvailable_Of.close();
	seatsAvailable_Of.open("seats_available.txt", ios::trunc);
	for(int i = 0; i < MAX_EVENTS; i++)
	{
		for(int j = 0; j < MAX_VIEWERS; j++)
			for(int k = 0; k < MAX_VIEWERS; k++)
				seatsAvailable_Of << seats[i][j][k] << " ";
		seatsAvailable_Of << endl;
	}
}

void updateStatistics()
{
	database_If.close();
	database_If.open("database.txt");
	string line;
	statistics_Of.close();
	statistics_Of.open("statistics.txt", ios::trunc);
	int counter =0;
	while(getline(database_If, line)&&counter < getCountOfEvents())
	{
		statistics_Of << line.substr(3, line.find(" ***")-3) << " - " << boughtTickets[counter] << endl;
		counter++;
	}
}

int getCountOfEvents()
{
	string line;
	eventsCount_If.close();
	eventsCount_If.open("events_count.txt");
	getline(eventsCount_If, line);
	return atoi(line.c_str());
}

void showMainScreen()
{
	//refresh file streams  
	database_If.close();
	database_If.open("database.txt");
	eventsCount_If.close();
	eventsCount_If.open("events_count.txt");
	validTickets_If.close();
	validTickets_If.open("valid_tickets.txt");
	seatsAvailable_If.close();
	seatsAvailable_If.open("seats_available.txt");
	bookedTickets_If.close();
	bookedTickets_If.open("booked_tickets.txt");
	statistics_If.close();
	statistics_If.open("statistics.txt");

	cout << "Select an option: "<< endl;
	cout << "1.Add new event" << endl;
	cout << "2.Available seats" << endl;
	cout << "3.Book a ticket" << endl;
	cout << "4.Buy a ticket" << endl;
	cout << "5.Booked seats" << endl;
	cout << "6.Cancel reservation" << endl;
	cout << "7.Check ticket validity" << endl;
	cout << "8.Recent events" << endl;
	cout << "9.Statistics" << endl;

	int input=0;
	do{
		cin >> input;
	}while(input <1 || input >11);
	cin.sync();

	if(is_file_empty(eventsCount_If) && input >1)
	{
		cout << "No events yet" << endl;
		system("pause");
		showMainScreen();
	}
	switch(input)
	{
	case 1:
		newEvent();
		break;
	case 2:
		showAvailableSeats();
		break;
	case 3:
		bookTicket();
		break;
	case 4:
		buyTicket();
		break;
	case 5:
		showBookedSeats();
		system("pause");
		showMainScreen();
		break;
	case 6:
		cancelReservation();
		break;
	case 7:
		checkTicketValidity();
		break;
	case 8:
		showRecentEvents();
		break;
	case 9:
		showStatistics();
		break;
	
	}
}

int main()
{
	//open text file streams to read from
	database_If.open("database.txt");
	eventsCount_If.open("events_count.txt");
	validTickets_If.open("valid_tickets.txt");
	seatsAvailable_If.open("seats_available.txt");
	bookedTickets_If.open("booked_tickets.txt");
	statistics_If.open("statistics.txt");

	//do this only if it's the first time the program starts or seats_available.txt is missing
	if(is_file_empty(seatsAvailable_If))
	{
		for(int i = 0; i < MAX_EVENTS; i++)
		{
			for(int j = 0; j < MAX_VIEWERS*MAX_VIEWERS; j++)
				seatsAvailable_Of << 0 << " ";
			seatsAvailable_Of << endl;
		}
		seatsAvailable_Of.flush();
	}else{ //else if seats_available.txt exists populate the seats array [][][]
		seatsAvailable_If.close();
		seatsAvailable_If.open("seats_available.txt");
		for(int i =0; i < MAX_EVENTS; i++)
		{
			string line;
			getline(seatsAvailable_If, line);
			for(int j = 0; j < MAX_VIEWERS; j++)
			{
				for(int k = 0; k < MAX_VIEWERS; k++)
				{
					if(line[j*20 + k*2] == '1')
						seats[i][j][k] = 1;
					else if(line[j*20 + k*2] == '2')
						seats[i][j][k] = 2;
				}
			}
		}
	}

	if(!is_file_empty(eventsCount_If))
	{
		for(int i = 0; i < getCountOfEvents(); i++)
		{
			string line;
			getline(statistics_If, line);
			string count;
			for(int j = line.length()-1; j>0; j--)
			{
				if(line[j] != '-')
					count += line[j];
				else
					break;
			}
			boughtTickets[i] = atoi(count.c_str());		
		}
	}
	/* for debugging purposes outputs all seats
	for(int i =0; i < MAX_EVENTS; i++)
	{
		for(int j = 0; j < MAX_VIEWERS; j++)
		{
			for(int k = 0; k < MAX_VIEWERS; k++)
			{
				cout << seats[i][j][k] << " ";
			}
		}
	} */

	//initial call of main program loop
	showMainScreen();

	system("pause");
	//file streams close automatically upon program exit
	return 0;
}