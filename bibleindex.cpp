/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * updated by Deborah Wilson, February 2026
 * Mount Vernon Nazarene University
 *
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 * refers to the actual string entered in the form's "verse" field.
 *
 * STUDENT NAME: Elijah Litman
 */

#include <iostream>
#include <stdio.h>
#include <string.h>

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

// Import FIFO
#include "fifo.h"

using namespace std;
using namespace cgicc;

// Set up logging
#define logging
   #define LOG_FILENAME "/home/class/csc3004/tmp/elilitman-bibleindex.log"
#include "logfile.h"

// Name pipes
string send_pipe = "BibleRequest";
string receive_pipe = "BibleReply";

int main() {
   #ifdef logging
      logFile.open(LOG_FILENAME, ios::out);
   #endif
   /* A CGI program must send a response header with content type
    * back to the web client before any other output.
    * For an AJAX request, our response is not a complete HTML document,
    * so the response type is just plain text to insert into the web page.
    */
   cout << "Content-Type: text/plain\n\n";

   log("Program Started");

   Cgicc cgi;  // create object used to access CGI request data

   // Create the FIFO pipes
   Fifo recfifo(receive_pipe);
   Fifo sendfifo(send_pipe);

   // GET THE INPUT DATA
   // browser sends us a string of field name/value pairs from HTML form
   // retrieve the value for each appropriate field name
   form_iterator st = cgi.getElement("search_type");
   form_iterator book = cgi.getElement("book");
   form_iterator chapter = cgi.getElement("chapter");
   form_iterator verse = cgi.getElement("verse");
   form_iterator nv = cgi.getElement("num_verse");

   // Convert book, chapter, and verse to ints
   int bookNum = book->getIntegerValue();
   int chapterNum = chapter->getIntegerValue();
   int verseNum = verse->getIntegerValue();
   int numVerses = nv->getIntegerValue();

   // TODO: OTHER INPUT VALUE CHECKS ARE NEEDED ... but that's up to you!
   // For checking input data
   bool validRefInput = true;

   /* Constants for checking the input data
    * A constant is used to avoid magic numbers
    */
   const int LARGEST_BOOK_NUM = 66;
   const int LARGEST_CHAPTER_NUM = 150;
   const int LARGEST_VERSE_NUM = 176;

   // Book number check
   if (book != cgi.getElements().end()) {
      if (bookNum > LARGEST_BOOK_NUM || bookNum <= 0) {
         cout << "<p>There is no book number " << bookNum << " in the Bible.</p>" << endl;
         validRefInput = false;
      }
   }

   // Chapter number check
   if (chapter != cgi.getElements().end()) {
      if (chapterNum > LARGEST_CHAPTER_NUM) {
         cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
         validRefInput = false;
      } else if (chapterNum <= 0) {
         cout << "<p>The chapter must be a positive number.</p>" << endl;
         validRefInput = false;
      }
   }

   // Verse number check
   if (verse != cgi.getElements().end()) {
      if (verseNum > LARGEST_VERSE_NUM) {
         cout << "<p>The verse number (" << verseNum << ") is too high.</p>" << endl;
         validRefInput = false;
      } else if (verseNum <= 0) {
         cout << "<p>The verse must be a positive number.</p>" << endl;
         validRefInput = false;
      }
   }

   /* TODO: PUT CODE HERE TO CALL YOUR BIBLE CLASS FUNCTIONS
    *        TO LOOK UP THE REQUESTED VERSES
    */
   if (validRefInput) {

      // Ensure numVerses is at least 1
      if (nv == cgi.getElements().end() || numVerses < 1) {
         numVerses = 1;
      }

      // Create and send the request string
      string requestedPassage = to_string(bookNum) + "|" + to_string(chapterNum) + "|" + to_string(verseNum) + "|" + to_string(numVerses);
      log("Received: " + requestedPassage);

      sendfifo.openwrite();
      log("Opened sendfifo");

      sendfifo.send(requestedPassage);
      log("Sent: " + requestedPassage + " on pipe");

      // Get the results
      recfifo.openread();
      log("Opened recfifo");

      string returnedMessage = "";

      // Display the results
      while (returnedMessage != "$end") {
         returnedMessage = recfifo.recv();

         if (returnedMessage.find("$") == string::npos) {
            cout << "<p>" << returnedMessage << "</p>" << endl;
         }

         log("Received: " + returnedMessage);
      }

      // Close the pipes
      recfifo.fifoclose();
      log("Closed recfifo");

      sendfifo.fifoclose();
      log("Closed sendfifo");
   }

   return 0;
}
