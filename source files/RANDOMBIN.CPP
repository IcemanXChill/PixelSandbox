#include "Randombin.h"

Randombin::Randombin(int min, int max, int amount)
	:dist(min, max)
{
	


}

int Randombin::getnum()
{
	return dist(m_engine);
}
