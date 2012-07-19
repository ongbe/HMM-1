/*!<
  \file martian.cpp
  \brief example usage of HMM library: a Martian learned basic English Language (vowels and consonants) 
  \author DOP (dohmatob elvis dopgima)
*/

#include "HMM.hpp" // pull-in HiddenMarkovModels namespace, etc.
#include "HMMBasicTypes.hpp"  // pull-in RealType, etc.
#include "HMMUtils.hpp" // pull-in load_hmm_matrix, load_hmm_vector, etc.
#include <algorithm> // random_shuffle, etc.
#include <ctype.h> //pull-in __toascii, etc.
#include <stdio.h> // pull-in printf, etc.

using namespace HiddenMarkovModels;

int main(int argc, const char *argv[])
{
  // prepare data
  // XXX refactor main into unittest cases
  std::cout << "Loadin: HMM parameters from files .." << std::endl;
  matrix trans = load_hmm_matrix("data/corpus_transition.dat");
  matrix em = load_hmm_matrix("data/corpus_emission.dat");
  vector pi = load_hmm_vector("data/corpus_pi.dat");
  std::cout << "Done." << std::endl << std::endl;

  // initialize HMM object
  HMM hmm(trans, em, pi);
  std::cout << "HMM:" << std::endl << hmm;

  // prepare data
  std::cout << std::endl << "Loadin: english words from corpus file .." << std::endl;
  std::vector<SequenceType> corpus = load_hmm_observations("data/corpus_words.dat"); // load
  std::cout << "Done (loaded " << corpus.size() << " words)." << std::endl << std::endl;

  // draw a random sample
  int nlessons = 1000;
  std::cout << "Sampling " << nlessons << " words from corpus .." << std::endl;
  std::random_shuffle(corpus.begin(), corpus.end());
  std::vector<SequenceType> lessons = std::vector<SequenceType>(corpus.begin(), corpus.begin()+nlessons%corpus.size());
  std::cout << "Done." << std::endl << std::endl;

  // run Viterbi
  boost::tuple<SequenceType,
	       RealType
	       > path = hmm.viterbi(lessons[2]);

  std::cout << "The a posteriori most probable sequence of hidden states that generated the trace " << lessons[2] << " is " << boost::get<0>(path) << "." << std::endl;
  std::cout << "Its (log) likelihood is " << boost::get<1>(path) << ".\n" << std::endl;

  // run Bauw-Welch
  hmm.baum_welch(lessons);
  std::cout << std::endl << "Final HMM:" << std::endl << hmm;
  std::cout << "Viterbi classification of the 26 symbols (cf. letters of the english alphabet):" << std::endl;
  SequenceType seq(1);
  unsigned int correction;
  for (int symbol = 0; symbol < 26; symbol++)
    {
      seq[0] = symbol;
      boost::tuple<SequenceType,
		   RealType
		   > path = hmm.viterbi(seq);

      unsigned int which = boost::get<0>(path)[0]; // vowel or consonant ?

      // let's call letter A's cluster "vowel" and call the other cluster "consonant"
      correction = symbol ? correction : which;
      which = correction ? 1-which : which;

      printf("\t%c is a %s\n", __toascii('A')+symbol, which ? "consonant" : "vowel");
    }

  return 0;
}
