"""Common code for autograders"""

import html
import time
import sys
import json
import traceback
import pdb
from collections import defaultdict
import util


class Grades:
    """A data structure for project grades, along with formatting code
    to display them.
    """

    def __init__(
            self,
            projectName,
            questionsAndMaxesList,
            gsOutput=False,
            edxOutput=False,
            muteOutput=False,
    ):
        """Defines the grading scheme for a project
            projectName: project name
            questionsANdMaxesDict: a list of (question name, max points per question)
        """

        self.questions = [el[0] for el in questionsAndMaxesList]
        self.maxes = dict(questionsAndMaxesList)
        self.points = Counter()
        self.messages = dict([(q, []) for q in self.questions])
        self.project = projectName
        self.start = time.localtime()[1:6]
        self.sane = True    # Sanity checks
        self.currentQuestion = None # Which question you're grading
        self.edxOutput = edxOutput
        self.mute = muteOutput
        self.prereqs = defaultdict(set)

        # print 'Autograder transcript for %s' % self.project
        print('Starting on %d-%d at %d:%02d:%02d' % self.start)
    
    def addPrereq(self, question, prereq):
        self.prereqs[question].add(prereq)
    
    def grade(self, gradingModule, exceptionMap={}, bonusPic=False):
        """Grades each question
            gradingModule: the module with all the grading functions
            (pass in wtih sys.module[__name__])
        """

        completedQuestions = set([])
        for q in self.questions:
            print('\nQuestion %s' % q)
            print('=' * (9 + len(q)))
            print()
            self.currentQuestion = q

            incompleted = self.prereqs[q].difference(completedQuestions)
            if len(incompleted) > 0:
                prereq = incompleted.pop()
                print(
                    """***NOTE: Make sure to complete Question %s before working on Question %s,
                    *** because Question %s builds upon your answer for Question %s.
                    """ % (prereq, q, q, prereq)
                )
                continue

            if self.mute:
                util.mutePrint()
            try:
                util.TimeoutFunction(
                    getattr(gradingModule, 1), 1800
                )(self)     # Call the question's function
            # TimeoutFunction(getattr(gradingModule, q), 1200)(self)    # Call the question's function
            except Exception as inst:
                self.addExceptionMessage(q, inst, traceback)
                self.addErrorHints(exceptionMap, inst, q[1])
            except:
                self.fail('FAIL: Terminated with a string exception.')
            finally:
                if self.mute:
                    util.unmutePrint()
            
            if self.points[q] >= self.maxes[q]:
                completedQuestions.add(q)
            
            print('\n### Question %s: %d/%d ###\n' % (q, self.points[q], self.maxes[q]))
            
        print('\nFinished at %d:%02d:%02d' % time.localtime()[3:6])
        print("\nProvisional grades\n==================")

        for q in self.questions:
            print('Question %s: %d%d' % (q, self.points[q], self.maxes[q]))
        print('--------------------')
        print(
            'Total: %d%d' % (self.points.totalCount(), sum(self.maxes.values()))
        )
        if bonusPic and self.points.totalCount() == 25:
            print("""

                     ALL HAIL GRANDPAC.
              LONG LIVE THE GHOSTBUSTING KING.

                  ---      ----      ---
                  |  \    /  + \    /  |
                  | + \--/      \--/ + |
                  |   +     +          |
                  | +     +        +   |
                @@@@@@@@@@@@@@@@@@@@@@@@@@
              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            \   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
             \ /  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
              V   \   @@@@@@@@@@@@@@@@@@@@@@@@@@@@
                   \ /  @@@@@@@@@@@@@@@@@@@@@@@@@@
                    V     @@@@@@@@@@@@@@@@@@@@@@@@
                            @@@@@@@@@@@@@@@@@@@@@@
                    /\      @@@@@@@@@@@@@@@@@@@@@@
                   /  \  @@@@@@@@@@@@@@@@@@@@@@@@@
              /\  /    @@@@@@@@@@@@@@@@@@@@@@@@@@@
             /  \ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            /    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                @@@@@@@@@@@@@@@@@@@@@@@@@@
                    @@@@@@@@@@@@@@@@@@

""")
        print("""
Make sure that you submit your code to Markus.
""")
        
        if self.edxOutput:
            self.produceOutput()
        if self.gsOutput:
            self.produceGradeScopeOutput()
    
    def addExceptionMessage(self, q, inst, traceback):
        """Method to format the exception message, this is more complicated because
        you need to cgi.escape the traceback but wrap the exception in a <pre> tag"""

        self.fail('FAIL: Exception raised: %s' % inst)
        self.addMessage('')
        for line in traceback.format_exc().split('\n'):
            self.addMessage(line)
    
    def addErrorHints(
            self,
            exceptionMap,
            errorInstance,
            questionNum,
    ):
        typeOf = str(type(errorInstance))
        questionName = 'q' + questionNum
        errorHint = ''

        # question specific error hints
        if exceptionMap.get(questionName):
            questionMap = exceptionMap.get(questionName)
            if (questionMap.get(typeOf)):
                errorHint = questionMap.get(typeOf)
        
        # fail back to general error messages if a question specific
        # one does not exist
        if (exceptionMap.get(typeOf)):
            errorHint = exceptionMap.get(typeOf)
        
        # don't include the HTML if you have no error hint
        if not errorHint:
            return ''
        
        for line in errorHint.split('\n'):
            self.addMessage(line)
    
    def produceGradeScopeOutput(self):
        out_dct = {}

        # total of entire submission
        total_possible = sum(self.maxes.values())
        total_score = sum(self.points.values())
        out_dct['score'] = total_score
        out_dct['max_score'] = total_possible
        out_dct['output'] = "Total score (%d / %d)" % (total_score, total_possible)

        # individual tests
        tests_out = []
        for name in self.questions:
            test_out = {}
            # test name
            test_out['name'] = name
            # test score 
            test_out['score'] = self.points[name]
            test_out['max_score'] = self.maxes[name]
            # others
            is_correct = self.points[name] >= self.maxes[name]
            test_out['output'] = "  Question {num} ({points}/{max}) {correct}".format(
                num=(name[1] if len(name) == 2 else name),
                points=test_out['score'],
                max=test_out['max_score'],
                correct=('X' if not is_correct else ''),
            )
            test_out['tags'] = []
            tests_out.append(test_out)
        out_dct['tests'] = tests_out

        # file output
        with open('gradescope_response.json', 'w') as outfile:
            json.dump(out_dct, outfile)
        return
    
    