#include "AssertException.h"
#include <cstring>

namespace UnitTest {

AssertException::AssertException(char const* description, char const* filename, int lineNumber)
    : m_lineNumber(lineNumber)
{
	using namespace std;

#if defined(_MSC_VER)
    strcpy_s(m_description, ASSERT_EXCEPTION_DESCRIPTION_LENGTH, description);
    strcpy_s(m_filename, ASSERT_EXCEPTION_FILENAME_LENGTH, filename);
#else
    strcpy(m_description, description);
    strcpy(m_filename, filename);
#endif // defined(_MSC_VER)
}

AssertException::~AssertException() throw()
{
}

char const* AssertException::what() const throw()
{
    return m_description;
}

char const* AssertException::Filename() const
{
    return m_filename;
}

int AssertException::LineNumber() const
{
    return m_lineNumber;
}

}
