#!/usr/bin/env python3

import sys
import threading
import queue
import os
import subprocess as sp
import math
import difflib

from multiprocessing import Lock

SUFFIX_INPUT = "_input"
SUFFIX_COMPARE = "_compare"
SUFFIX_LOG = "_log.dat"

def get_log_name(level):
    return "%s%s" % (level, SUFFIX_LOG)

# static class
class Logger():
    debug_level = 1
    DEBUG = 0
    INFO = 1
    WARNING = 2
    CRITICAL = 3
    RESULTS = 4
    log_lock = Lock()

    messages = ("DEBUG", "INFO", "WARNING", "CRITICAL", "RESULTS")

    @staticmethod
    def log(msg, level, prepend=""):
        if level < Logger.debug_level: return

        Logger.log_lock.acquire()
        print("%s%s: %s" % (prepend, Logger.messages[level], msg), file=sys.stderr)
        Logger.log_lock.release()


class Runner(threading.Thread):
    queue = queue.Queue(1)
    
    def __init__(self, tid):
        threading.Thread.__init__(self)
        self.tid = tid
        self.base_folder = os.getcwd()
        
    def run(self):
        while True:
            details = Runner.queue.get(True)
            
            system = details["system"]
            to_execute = system.executable
            folder = system.folder
            
            try:
                p = sp.Popen(to_execute, shell=True, stdout=sp.PIPE, stderr=sp.PIPE, cwd=folder, universal_newlines=True)
                p.wait()
                system.simulation_done(p)
                
            except Exception as e:
                Logger.log(e, Logger.WARNING)
            
            Runner.queue.task_done()
            
            
class BaseTest(object):
    def __init__(self, folder, log_prefix, parameters):
        self.log_prefix = log_prefix
        self.folder = folder
        self.parameters = parameters
        self.error = False
        self.parse_parameters()
        
    def parse_parameters(self):
        Logger.log("%s %s's validate_parameters() not implemented" % (self.log_prefix, type(self)), Logger.CRITICAL)
        sys.exit(1)
    
    def test(self):
        Logger.log("%s %s's test() not implemented" % (self.log_prefix, type(self)), Logger.CRITICAL)
        sys.exit(1)
    
    def generate_compare_line(self):
        Logger.log("%s %s's generate_compare_line() not implemented" % (self.log_prefix, type(self)), Logger.CRITICAL)
        sys.exit(1)
    
    
class DiffFiles(BaseTest):
    def __init__(self, folder, log_prefix, parameters):
        BaseTest.__init__(self, folder, log_prefix, parameters)
        
    def parse_parameters(self):
        if len(self.parameters) != 3:
            Logger.log("%s ColumnAverage expects 2 parameters: the reference and data files" % self.log_prefix, Logger.WARNING)
            self.error = True
        else:
            self.reference_file = os.path.join(self.folder, self.parameters[1])
            self.data_file = os.path.join(self.folder, self.parameters[2])
    
    def test(self):
        if not self.error:
            with open(self.reference_file) as ref_file:
                ref_data = ref_file.readlines()
            with open(self.data_file) as data_file:
                data = data_file.readlines()
    
        return len(list(difflib.unified_diff(ref_data, data))) == 0
    
    
class ColumnAverage(BaseTest):
    def __init__(self, folder, log_prefix, parameters):
        BaseTest.__init__(self, folder, log_prefix, parameters)
            
    def parse_parameters(self):
        if len(self.parameters) != 5:
            Logger.log("%s ColumnAverage expects 4 parameters: the reference file, the column index, the reference value and its associated error" % self.log_prefix, Logger.WARNING)
            self.error = True
            
        if not self.error:
            self.filename = os.path.abspath(os.path.join(self.folder, self.parameters[1]))
            self.column = int(self.parameters[2]) - 1
            self.true_avg = float(self.parameters[3])
            self.tolerance = float(self.parameters[4])
            
    def _get_average(self):
        if not os.path.exists(self.filename):
            Logger.log("%s ColumnAverage expects the reference file '%s' to exist and be readable" % (self.log_prefix, self.filename), Logger.WARNING)
            self.error = True
            return 0., 0.
        
        avg = 0.
        avg2 = 0.
        c = 0
        
        f = open(self.filename, "r")
        for l in f.readlines():
            v = float(l.split()[self.column])
            avg += v
            avg2 += v*v
            c += 1
        f.close()
        
        avg /= c
        avg2 /= c
        sigma = avg2 - avg*avg
        
        return avg, math.sqrt(sigma)
        
    def test(self):
        avg, error = self._get_average()
        
        if self.error: 
            return False
        
        lower_b = self.true_avg - self.tolerance
        higher_b = self.true_avg + self.tolerance
        
        if lower_b <= avg <= higher_b: 
            Logger.log("%s ColumnAverage test on file '%s', column %d passed. Compute value: %f, Reference value: %f (+- %f)" % (self.log_prefix, self.filename, self.column, avg, self.true_avg, self.tolerance), Logger.DEBUG)
            return True
        
        Logger.log("%s ColumnAverage test on file '%s', column %d failed. Compute value: %f, Reference value: %f (+- %f)" % (self.log_prefix, self.filename, self.column, avg, self.true_avg, self.tolerance), Logger.WARNING)
        
        return False
    
    def generate_compare_line(self):
        avg, error = self._get_average()
        
        spl_line = self.parameters[:]
        spl_line[3] = avg
        spl_line[4] = error
        
        return "::".join(str(x) for x in spl_line)
    
    
class Analyser(object):
    def __init__(self, folder, compare_file):
        self.log_prefix = "Analyser '%s':" % folder
        Logger.log("%s initialising" % self.log_prefix, Logger.DEBUG)

        self.folder = folder        
        self.compare_file = os.path.abspath(os.path.join(folder, compare_file))
        if not os.path.exists(self.compare_file) or not os.path.isfile(self.compare_file):
            Logger.log("%s compare file '%s' not found" % (self.log_prefix, self.compare_file), Logger.CRITICAL)
            sys.exit(1)

        self.parse_compare_file()
        
    def parse_compare_file(self):
        self.tests = []
        cmp = open(self.compare_file, "r")
        for l in cmp.readlines():
            spl = l.strip().split("::")
            if len(spl) > 0:
                # get an instance of the required test class
                try:
                    test_class = globals()[spl[0]]
                except KeyError as e:
                    Logger.log("%s test class '%s' not found" % (self.log_prefix, spl[0]), Logger.WARNING)
                    continue
                
                self.tests.append(test_class(self.folder, self.log_prefix, spl))
                
        cmp.close()
        
    def print_compare_file(self):
        lines = []
        for test in self.tests:
            lines.append(test.generate_compare_line())
            
        cmp = open(self.compare_file, "w")
        cmp.write("\n".join(lines))
        cmp.close()
            
    
    def test(self):
        n_tests = len(self.tests)
        n_failed = 0
        
        for test in self.tests:
            if not test.test(): n_failed += 1
        
        return (n_tests, n_failed)


class System(object):
    def __init__(self, folder, exec_name, compare_file):
        self.log_prefix = "System '%s':" % folder
        
        Logger.log("%s initialising" % self.log_prefix, Logger.DEBUG)
        
        self.executable = exec_name
        self.folder = os.path.abspath(folder)
        
        self.analyser = Analyser(folder, compare_file)
        
        self.n_tests = 0
        self.n_failed = 0
    
    def simulation_done(self, p, do_tests=True):
        error = False
        if p.returncode != 0:
            # segfault
            if p.returncode == 139: 
                Logger.log("%s segmentation fault (return code %d)" % (self.log_prefix, p.returncode), Logger.WARNING)
            else: 
                Logger.log("%s %s returned %d and the following output:" % (self.log_prefix, self.executable, p.returncode), Logger.WARNING)
                for line in p.stderr:
                    print("\t" + line.strip("\n"))
            error = True

        # we don't run tests if the simulation was not successful. We put this here so that all
        # above messages can be printed independently of each other
        if error: 
            return
        
        Logger.log("%s %s run completed and successful" % (self.log_prefix, self.executable), Logger.DEBUG)
        
        if do_tests:
            (n_tests, n_failed) = self.analyser.test()
            Logger.log("%s\n\tFailed/Total: %d/%d" % (self.log_prefix, n_failed, n_tests), Logger.RESULTS)
            
            self.n_tests = n_tests
            self.n_failed = n_failed
        
    
class TestManager(object):
    def __init__(self, test_list_file, threads=1):
        self.log_prefix = "TestManager:"
        
        self.systems = []
        self.threads = threads
            
        f = open(test_list_file, "r")
        for line in f.readlines():
            line = line.strip()
            if len(line) == 0 or line[0] == '#':
                continue
            split_line = [x.strip() for x in line.split()]
            test_dir = split_line[0]
            test_pairs = split_line[1:]
            
            for test_pair in test_pairs:
                try:
                    test_executable, test_compare_file = test_pair.split(":")
                except:
                    print("Invalid test pair '%s'" % test_pair, file=sys.stderr)
                    return
                
                new_system = System(test_dir, test_executable, test_compare_file)
                self.systems.append(new_system)

        f.close()
                
    def launch(self):
        for i in range(self.threads):
            runner = Runner(i)
            runner.setDaemon(True)
            runner.start()
        
        for system in self.systems:
            Logger.log("%s starting %s in '%s'" % (self.log_prefix, system.executable, system.folder), Logger.DEBUG)
            
            details = {
                       "system" : system,
                       }
            
            Runner.queue.put(details, block=True)
            
        Runner.queue.join()
        
    def finalise(self):
        n_failed = 0
        n_tests = 0
        for system in self.systems:
            n_failed += system.n_failed
            n_tests += system.n_tests
            
        Logger.log("Summary:\n\tFAILED/TOTAL: %d/%d\n" % (n_failed, n_tests), Logger.RESULTS, "\n")
    
    
def main():
    def print_usage():
        print("USAGE:")
        print("\t%s test_list_file [-d|--debug] [-h|--help] [-v|--version] [--threads=N_threads]" % sys.argv[0])
        exit(1)

    def print_version():
        print("baggianalysis Test Suite v 0.1")
        print("This is free software; see the source for copying conditions.  There is NO")
        print("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n")
        exit(1)

    shortArgs = 'dhv'
    longArgs = ['debug', 'help', 'version', 'threads=']
    
    threads = 1
    
    try:
        import getopt
        args, files = getopt.gnu_getopt(sys.argv[1:], shortArgs, longArgs)
        for k in args:
            if k[0] == '-d' or k[0] == '--debug': Logger.debug_level = 0
            if k[0] == '-h' or k[0] == '--help': print_usage()
            if k[0] == '-v' or k[0] == '--version': print_version()
            if k[0] == '--threads': threads = int(k[1])
            
    except Exception as e:
        print(e)
        print_usage()
        
    if len(sys.argv) < 2:
        print_usage()
        exit(1)
        
    file_list = sys.argv[1]
    if not os.path.exists(file_list) or not os.path.isfile(file_list):
        Logger.log("List file '%s' does not exist or it is unreadable" % file_list, Logger.CRITICAL)
        sys.exit(1)
        
    tm = TestManager(sys.argv[1], threads=threads)
    tm.launch()
    tm.finalise()

    
if __name__ == '__main__':
    main()
