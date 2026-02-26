/* testreader.cpp
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * main function for Project 1
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Elijah litman
 */

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main (int argc, char **argv) {
   // Check for at least 3 parameters
   if (argc < 4) {
      if (argc <= 1) {
         cerr << "Error: book, chapter, and verse numbers are missing" << endl;
      } else if (argc == 2) {
         cerr << "Error: chapter and verse numbers are missing" << endl;
      } else {
         cerr << "Error: verse number is missing" << endl;
      }

      return 1;
   }

   // Create Bible object to process the raw text file
   Bible webBible("/home/class/csc3004/Bibles/web-complete");

   Verse verse;
   int bookNum, chapterNum, verseNum, numVerses;
   LookupResult result;

   // For debugging
   /*cout << "Using Bible from: ";
   webBible.display();*/

   // Prompt for input: get reference to lookup
   // This is good for testing your classes quickly for the first milestone.
   /*cout << "Enter 3 integers for book, chapter and verse to find: " << flush;
   cin >> bookNum >> chapterNum >> verseNum;*/

   // TODO: your final program should get input from command line arguments instead.
   bookNum = atoi(argv[1]);
   chapterNum = atoi(argv[2]);
   verseNum = atoi(argv[3]);

   if (argc > 4 && atoi(argv[4]) > 0) {
      numVerses = atoi(argv[4]);
   } else {
      numVerses = 1;
   }

   // Create a reference from the numbers
   Ref ref(bookNum, chapterNum, verseNum);

   // Use the Bible object to retrieve the verse by reference
   // For debugging
   /*cout << "Looking up reference: ";
   ref.display();
   cout << endl;*/

   // Open the Bible
   webBible.openBible();

   // Find the verse
   verse = webBible.lookup(ref, result);

   if (result == SUCCESS) {
      verse.display();
      cout << endl;

      for (int i = 0; i < numVerses - 1; i++) {

         // Do not allow any attempts to retrieve a verse beyond Rev 22:21
         if (verse.getRef().getBook() == 66 &&
             verse.getRef().getChapter() == 22 &&
             verse.getRef().getVerse() == 21) {
           cout << "Revelation 22:21 is the last verse in the Bible." << endl;
           break;
         }

         // Get the next verse
         Verse nextVerse = webBible.nextVerse(result);

         // Determine if the book and chapter needs to be displayed or not
         if (nextVerse.getRef().getBook() > verse.getRef().getBook() ||
             nextVerse.getRef().getChapter() > verse.getRef().getChapter()) {
            cout << endl;
            nextVerse.display();
         } else {
            nextVerse.displayNoBookOrChapter();
         }

         cout << endl;

         /* Set verse to nextVerse
            This is to help determine if
            display or displayNoBC should be called */
         verse = nextVerse;
      }
   } else {
      // If the initial verse does not exist, display the error
      cout << webBible.error(ref, result) << endl;
   }

   // Close the Bible
   webBible.closeBible();

   // For debugging
   //cout << "Result status: " << result << endl;
}
