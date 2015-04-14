void showMainScreen();
void newEvent();
void bookTicket();
void buyTicket();
void cancelReservation();
void checkTicketValidity();
void showBookedSeats();
void showRecentEvents();
void showStatistics();
void showAvailableSeats();
void printEvents();
void updateStatistics();
int getCountOfEvents();
void saveChangesToSeats();
bool is_file_empty(std::ifstream& pFile);
void getLineNumber(std::ifstream& file, int num, std::string& line);
int lineCount(std::ifstream& file);

//std::ifstream& GotoLine(std::ifstream& file, unsigned int num);
