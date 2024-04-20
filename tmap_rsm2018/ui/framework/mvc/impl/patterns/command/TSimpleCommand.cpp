#include "TSimpleCommand.h"

using namespace SKT;
using namespace SKT::mvc;

TSimpleCommand::TSimpleCommand()
{
}

TSimpleCommand::~TSimpleCommand()
{
}

bool TSimpleCommand::execute( const INotificationPtr& note )
{
    return false;
}
