AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  CaloTowerBuilderFromHisto.h

lib_LTLIBRARIES = \
  libCaloTowerBuilderFromHisto.la

libCaloTowerBuilderFromHisto_la_SOURCES = \
  CaloTowerBuilderFromHisto.cc

libCaloTowerBuilderFromHisto_la_LIBADD = \
  -lcalo_io \
  -lffamodules \
  -lffarawobjects \
  -lgsl \
  -lgslcblas \
  -lcalo_reco \
  -lphool \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libCaloTowerBuilderFromHisto.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
