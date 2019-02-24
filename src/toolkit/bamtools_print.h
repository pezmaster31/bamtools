// ***************************************************************************
// bamtools_print.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 24 Feb 2019
// ---------------------------------------------------------------------------
// Text printout of alignments at a particular reference location
// ***************************************************************************

#ifndef BAMTOOLS_PRINT_H
#define BAMTOOLS_PRINT_H

#include "bamtools_tool.h"

namespace BamTools {

class PrintTool : public AbstractTool
{

public:
    PrintTool();
    ~PrintTool();

public:
    int Help();
    int Run(int argc, char* argv[]);

private:
    struct PrintSettings;
    PrintSettings* m_settings;

    struct PrintToolPrivate;
    PrintToolPrivate* m_impl;
};

}  // namespace BamTools

#endif  // BAMTOOLS_PRINT_H
