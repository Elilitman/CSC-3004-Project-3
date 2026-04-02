/* lookupserver.cpp
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * Main function for Project 3
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Elijah litman
 */

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include "fifo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Returns a string version of a LookupResult variable
string resultToString(LookupResult result) {
   switch (result) {
      case LookupResult::SUCCESS:
         return "SUCCESS";
      case LookupResult::NO_BOOK:
         return "NO_BOOK";
      case LookupResult::NO_CHAPTER:
         return "NO_CHAPTER";
      case LookupResult::NO_VERSE:
         return "NO_VERSE";
      default:
         return "OTHER";
   }
}

int main () {
   // Set up the pipes
   string receive_pipe = "BibleRequest";
   string send_pipe = "BibleReply";
   string message;

   Fifo recfifo(receive_pipe);
   Fifo sendfifo(send_pipe);

   // Create Bible object to process the raw text file
   Bible webBible("/home/class/csc3004/Bibles/web-complete");

   // Used for getting verses
   Verse verse;
   int bookNum, chapterNum, verseNum, numVerses;
   LookupResult result;

   // Open the pipes
   recfifo.openread();
   sendfifo.openwrite();

   while (true) {
      // Reset the result variable to avoid bugs
      result = SUCCESS;

      // Get the request
      string verseRequest = recfifo.recv();
      cout << "Received request: " << verseRequest << endl;

      // Parse the input parameters
      int inputParameters[4];

      try {
         for (int i = 0; i < 3; i++) {
            inputParameters[i] = stoi(verseRequest.substr(0, verseRequest.find("|")));
            verseRequest = verseRequest.substr(verseRequest.find("|") + 1);
         }

         inputParameters[3] = stoi(verseRequest);

      // Use Genesis 1:1 if anything goes wrong
      } catch (exception e) {
         inputParameters[0] = 1;
         inputParameters[1] = 1;
         inputParameters[2] = 1;
         inputParameters[3] = 1;
      }

      // Assign the inputted values
      bookNum = inputParameters[0];
      chapterNum = inputParameters[1];
      verseNum = inputParameters[2];
      numVerses = inputParameters[3];

      // Make the requested reference
      Ref requestedRef(bookNum, chapterNum, verseNum);

      // Get the requested verse
      webBible.openBible();
      verse = webBible.lookup(requestedRef, result);

      // Display the lookup results
      if (result == SUCCESS) {

         // Send the initial verse
         message = requestedRef.display() + " " + verse.getVerse();
         sendfifo.send(message);
         cout << message << endl;

         // Get the other verses (if any)
         for (int i = 0; i < numVerses - 1; i++) {

            // Do not allow any attempts to retrieve a verse beyond Rev 22:21
            if (verse.getRef().getBook() == 66 &&
                verse.getRef().getChapter() == 22 &&
                verse.getRef().getVerse() == 21) {
              message =  "Revelation 22:21 is the last verse in the Bible.";
              sendfifo.send(message);
              break;
            }

            // Get the next verse
            Verse nextVerse = webBible.nextVerse(result);

            // Determine if the book and chapter needs to be displayed or not
            if (nextVerse.getRef().getBook() > verse.getRef().getBook() ||
                nextVerse.getRef().getChapter() > verse.getRef().getChapter()) {
               message = "<br>" + nextVerse.display();
               sendfifo.send(message);
            } else {
               message = nextVerse.displayNoBookOrChapter();
               sendfifo.send(message);
            }

            /* Set verse to nextVerse
               This is to help determine if
               display or displayNoBC should be called*/
            verse = nextVerse;
         }

         // Send the status variable
         message = "$" + resultToString(result);
         sendfifo.send(message);
         cout <<  message << endl;
      } else {

         // If the initial verse does not exist, display the error
         message = webBible.error(requestedRef, result);
         sendfifo.send(message);
         cout << message << endl;

         // Send the status variable
         message = "$" + resultToString(result);
         sendfifo.send(message);
         cout << message << endl;
      }

      // Send the ending message
      message = "$end";
      cout << message << endl;
      sendfifo.send(message);
   }

   // Close the Bible
   webBible.closeBible();
}
