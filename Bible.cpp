/* Bible class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Elijah Litman
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

// Default constructor
Bible::Bible()
{
   infile = "/home/class/csc3004/Bibles/web-complete";
   buildBibleIndex(infile);
}

// Constructor – pass bible filename
Bible::Bible(const string s) { infile = s; buildBibleIndex(infile); }

// Create the index
int Bible::buildBibleIndex(string filename) {
   openBible();

   if (!instream.is_open()) {
      cerr << "Eror: cannot open the input file: " << filename << endl;
      return 0;
   }

   // Current position in the file
   int position = instream.tellg();

   // References added to the index
   int referenceCount = 0;

   /// Current line
   string line;

   // Add the references to the index
   while (getline(instream, line)) {
      instream.seekg(position);
      getline(instream, line);

      Ref ref = Ref(line);

      index[ref] = position;

      position = instream.tellg();
      referenceCount++;
   }

   // Display diagnostic information
   cout << "--Diagnostic Information--" << endl;

   // Display the total references added
   cout << "Number of refernces added: " << referenceCount << endl;

   // Display the byte offset of the last verse added
   auto it = std::prev(index.end());
   cout << "Byte offset of last verse added: " << it->second << endl;

   // Display the byte offset of a few specific verses
   Ref genesis1_1(1, 1, 1);
   Ref genesis1_2(1, 1, 2);
   Ref genesis1_27(1, 1, 27);

   cout << "Genesis 1:1 byte offset: " << index[genesis1_1] << endl;
   cout << "Genesis 1:2 byte offset: " << index[genesis1_2] << endl;
   cout << "Genesis 1:27 byte offset: " << index[genesis1_27] << endl;
   cout << endl;

   closeBible();

   return 1;
}

int Bible::validateRef(Ref ref, LookupResult& status) {
   // Check the book number's validity
   if (ref.getBook() < 1 || ref.getBook() > 66) {
      status = NO_BOOK;
      return 0;
   }

   // Check for verses after Revelation 22:21
   if (ref.getBook() == 66) {
      if (ref.getChapter() > 22) {
         status = NO_CHAPTER;
         return 0;
      }

      if (ref.getChapter() == 22 && ref.getVerse() > 21) {
         status = NO_VERSE;
         return 0;
      }
   }

   // Determine if the refernce does not exist
   if (index.count(ref) == 0) {
      Ref checkChapterValidity = Ref(ref.getBook(), ref.getChapter(), 1);

      if(index.count(checkChapterValidity) == 0) {
         status = NO_CHAPTER;
         return 0;
      } else {
         status = NO_VERSE;
         return 0;
      }
   }

   return 1;
}

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status) {
   // TODO: scan the file to retrieve the line that holds ref ...

   // Declare verse to be returned
   Verse aVerse;

   // Declare string to hold the current line
   string verseLine;

   // Check the ref's validity
   validateRef(ref, status);

   // Attempt to find the verse
   if ((status != NO_BOOK) && (status != NO_CHAPTER) &&
       (status != NO_VERSE)) {

      // Get the position
      int requestedVersePos = index[ref];

   if (!instream.is_open()) {
      openBible();
   }

      // Get the verse
      instream.clear();
      instream.seekg(requestedVersePos);
      getline(instream, verseLine);
      status = SUCCESS;
   }

   // update the status variable
   if (status == SUCCESS) {

      // Verse was found
      aVerse = Verse(verseLine);

   } else {
      // create and return the verse object
      // default verse, to be replaced by a Verse object
      // that is constructed from a line in the file.

      // Verse was not found
      aVerse = Verse();
   }

   return(aVerse);
}

// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
Verse Bible::nextVerse(LookupResult& status) {
   // Open the file if is not is open
   if (!instream.is_open()) {
      openBible();
   }

   // Verse the next (or first) verse and return it
   string verseNext;
   getline(instream, verseNext);

   Verse verse = Verse(verseNext);
   return verse;
}

// REQUIRED: Return an error message string to describe status
string Bible::error(Ref ref, LookupResult status) {
   if (status == NO_BOOK) {
      return "No such book " + to_string(ref.getBook());
   } else if (status == NO_CHAPTER) {
      return "No such chapter " + to_string(ref.getChapter())
              + " in " + ref.getBookName();
   } else if (status == NO_VERSE) {
      return "No such verse " + to_string(ref.getVerse()) + " in "
              + ref.getBookName() + " " + to_string(ref.getChapter());
   } else {
      return "Error: An unexpected error has occurred";
   }
}

void Bible::display() {
   cout << "Bible file: " << infile << endl;
}

// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status) {
   int position = index[ref];
   string verseLine;

   if (!instream.is_open()) {
      openBible();
   }

   instream.seekg(position);
   getline(instream, verseLine);
   getline(instream, verseLine);

   Ref nextRef = Ref(verseLine);

   return nextRef;
}

// OPTIONAL: Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult& status)
{
   return ref;
}

// Open the file
void Bible::openBible() {
   instream.open(infile);
}

// Close the file
void Bible::closeBible() {
   instream.close();
}
