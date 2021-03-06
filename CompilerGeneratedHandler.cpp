/******************************************************************************
 *
 * C++ Insights, copyright (C) by Andreas Fertig
 * Distributed under an MIT license. See LICENSE for details
 *
 ****************************************************************************/

#include "CompilerGeneratedHandler.h"
#include "DPrint.h"
#include "InsightsHelpers.h"
#include "InsightsMatchers.h"
#include "InsightsStaticStrings.h"
#include "OutputFormatHelper.h"
//-----------------------------------------------------------------------------

using namespace clang;
using namespace clang::ast_matchers;
//-----------------------------------------------------------------------------

namespace clang::insights {

CompilerGeneratedHandler::CompilerGeneratedHandler(Rewriter& rewrite, MatchFinder& matcher)
: InsightsBase(rewrite)
{
    static const auto compilerProvided = allOf(unless(isUserProvided()),
                                               unless(isDeleted()),
                                               unless(isExpansionInSystemHeader()),
                                               unless(isTemplate),
                                               unless(isMacroOrInvalidLocation()),
                                               hasParent(cxxRecordDecl().bind("record")));

    matcher.addMatcher(cxxMethodDecl(compilerProvided).bind("method"), this);
}
//-----------------------------------------------------------------------------

void CompilerGeneratedHandler::run(const MatchFinder::MatchResult& result)
{
    if(const auto* methodDecl = result.Nodes.getNodeAs<CXXMethodDecl>("method")) {
        OutputFormatHelper outputFormatHelper{};
        outputFormatHelper.Append("/* ");

        InsertAccessModifierAndNameWithReturnType(outputFormatHelper, *methodDecl);

        outputFormatHelper.AppendNewLine("; */");

        // add all compiler generated methods at the end of the class
        const auto* recrodDecl = result.Nodes.getNodeAs<CXXRecordDecl>("record");
        const auto  loc        = recrodDecl->getLocEnd();

        mRewrite.InsertText(loc, outputFormatHelper.GetString(), true, true);
    }
}
//-----------------------------------------------------------------------------

}  // namespace clang::insights
