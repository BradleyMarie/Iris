#ifndef UNITTEST_ASSERTEXCEPTION_H
#define UNITTEST_ASSERTEXCEPTION_H

#include <exception>

#define ASSERT_EXCEPTION_DESCRIPTION_LENGTH 512
#define ASSERT_EXCEPTION_FILENAME_LENGTH 512

namespace UnitTest {

class AssertException : public std::exception
{
public:
    AssertException(char const* description, char const* filename, int lineNumber);
    virtual ~AssertException() throw();

    virtual char const* what() const throw();

    char const* Filename() const;
    int LineNumber() const;

private:
    char m_description[ASSERT_EXCEPTION_DESCRIPTION_LENGTH];
    char m_filename[ASSERT_EXCEPTION_FILENAME_LENGTH];
    int m_lineNumber;
};

}

#endif
