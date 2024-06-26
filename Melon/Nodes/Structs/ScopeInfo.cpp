#pragma once

#include "ScopeInfo.h"

#include "Boxx/Math.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

void ScopeInfo::Reset() {
	hasReturned   = false;
	willNotReturn = true;

	loopAbortCount     = 0;
	loopBreakCount     = 0;
	scopeBreakCount    = 0;
	maxLoopBreakCount  = 0;
	maxScopeBreakCount = 0;
}

bool ScopeInfo::CanAbort() const {
	return loopAbortCount > 0 || maxLoopBreakCount > 1 || maxScopeBreakCount > 1;
}

bool ScopeInfo::WillBreak() const {
	return loopBreakCount > 0 || scopeBreakCount > 0;
}

bool ScopeInfo::WillReturn() const {
	return hasReturned;
}

bool ScopeInfo::WillContinue() const {
	return willNotReturn && maxLoopBreakCount == 0 && maxScopeBreakCount == 0;
}

bool ScopeInfo::CanContinue() const {
	return !WillReturn() && !WillBreak();
}

bool ScopeInfo::WillNotContinue() const {
	return WillReturn() || WillBreak();
}

void ScopeInfo::EnterScope(const ScopeType type) {
	types.Push(this->type);
	this->type = type;

	const bool isLoop = type == ScopeType::Loop;

	if (isLoop && loopAbortCount > 0)    loopAbortCount++;
	if (isLoop && loopBreakCount > 0)    loopBreakCount++;
	if (scopeBreakCount > 0)             scopeBreakCount++;
	if (isLoop && maxLoopBreakCount > 0) maxLoopBreakCount++;
	if (maxScopeBreakCount > 0)          maxScopeBreakCount++;
}

void ScopeInfo::ExitScope() {
	const bool isLoop = type == ScopeType::Loop;
	type = types.Pop();

	if (isLoop && loopAbortCount > 0)    loopAbortCount--;
	if (isLoop && loopBreakCount > 0)    loopBreakCount--;
	if (scopeBreakCount > 0)             scopeBreakCount--;
	if (isLoop && maxLoopBreakCount > 0) maxLoopBreakCount--;
	if (maxScopeBreakCount > 0)          maxScopeBreakCount--;
}

ScopeInfo ScopeInfo::CopyBranch() {
	ScopeInfo scopeInfo = *this;
	scopeInfo.unassigned = unassigned.Copy();
	scopeInfo.types = types.Copy();
	return scopeInfo;
}

ScopeInfo ScopeInfo::BranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2) {
	ScopeInfo result = branch1;
	result = result.CopyBranch();
	result.hasReturned   = branch1.hasReturned || branch2.hasReturned;
	result.willNotReturn = !result.hasReturned && branch1.willNotReturn && branch2.willNotReturn;

	result.loopAbortCount     = Math::Max(branch1.loopAbortCount,     branch2.loopAbortCount);
	result.loopBreakCount     = Math::Max(branch1.loopBreakCount,     branch2.loopBreakCount);
	result.scopeBreakCount    = Math::Max(branch1.scopeBreakCount,    branch2.scopeBreakCount);
	result.maxLoopBreakCount  = Math::Max(branch1.maxLoopBreakCount,  branch2.maxLoopBreakCount);
	result.maxScopeBreakCount = Math::Max(branch1.maxScopeBreakCount, branch2.maxLoopBreakCount);

	result.unassigned = Set<Name>::Intersection(branch1.unassigned, branch2.unassigned);
	//result.ownerVars  = Set<NameList>::Intersection(branch1.ownerVars, branch2.ownerVars);

	return result;
}

ScopeInfo ScopeInfo::WeakBranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2) {
	ScopeInfo result = branch1;
	result = result.CopyBranch();
	result.willNotReturn = !result.hasReturned && branch1.willNotReturn && branch2.willNotReturn;

	result.loopAbortCount     = Math::Max(branch1.loopAbortCount,     branch2.loopAbortCount);
	result.maxLoopBreakCount  = Math::Max(branch1.maxLoopBreakCount,  branch2.maxLoopBreakCount);
	result.maxScopeBreakCount = Math::Max(branch1.maxScopeBreakCount, branch2.maxLoopBreakCount);

	return result;
}

ScopeInfo ScopeInfo::BranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2) {
	ScopeInfo result = branch1;
	result = result.CopyBranch();
	result.hasReturned   = branch1.hasReturned && branch2.hasReturned;
	result.willNotReturn = !result.hasReturned && branch1.willNotReturn && branch2.willNotReturn;

	result.loopAbortCount     = Math::Max(branch1.loopAbortCount,     branch2.loopAbortCount);
	result.loopBreakCount     = Math::Min(branch1.loopBreakCount,     branch2.loopBreakCount);
	result.scopeBreakCount    = Math::Min(branch1.scopeBreakCount,    branch2.scopeBreakCount);
	result.maxLoopBreakCount  = Math::Max(branch1.maxLoopBreakCount,  branch2.maxLoopBreakCount);
	result.maxScopeBreakCount = Math::Max(branch1.maxScopeBreakCount, branch2.maxLoopBreakCount);

	result.unassigned = Set<Name>::Union(branch1.unassigned, branch2.unassigned);
	//result.ownerVars  = Set<NameList>::Union(branch1.ownerVars, branch2.ownerVars);

	return result;
}

ScopeInfo ScopeInfo::WeakBranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2) {
	ScopeInfo result = branch1;
	result = result.CopyBranch();
	result.willNotReturn = !result.hasReturned && branch1.willNotReturn && branch2.willNotReturn;

	result.loopAbortCount     = Math::Max(branch1.loopAbortCount,     branch2.loopAbortCount);
	result.maxLoopBreakCount  = Math::Max(branch1.maxLoopBreakCount,  branch2.maxLoopBreakCount);
	result.maxScopeBreakCount = Math::Max(branch1.maxScopeBreakCount, branch2.maxLoopBreakCount);

	return result;
}