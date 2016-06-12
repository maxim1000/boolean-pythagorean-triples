#include <chrono>
#include <iostream>
#include <vector>
enum class TColor
{
	Black,
	White,
	Undefined
};
bool IsCorrect(const std::vector<TColor> &colors)
{
	for(int a=1;a<int(colors.size());++a)
	{
		for(int b=a+1;b<int(colors.size());++b)
		{
			for(int c=b+1;c<int(colors.size());++c)
			{
				if(a*a+b*b==c*c && colors[a]==colors[b]&&colors[b]==colors[c])
					return false;
			}
		}
	}
	return true;
}
bool IsPossible(std::vector<TColor> &colors)
{
	const auto undefined=std::find(colors.begin(),colors.end(),TColor::Undefined);
	if(undefined==colors.end())
		return IsCorrect(colors);
	for(auto newColor:{TColor::Black,TColor::White})
	{
		*undefined=newColor;
		if(IsPossible(colors))
			return true;
	}
	*undefined=TColor::Undefined;
	return false;
}
int main()
{
	for(int max=1;max<8000;++max)
	{
		const auto start=std::chrono::steady_clock::now();
		std::cout<<max<<": ";
		std::vector<TColor> colors(max+1,TColor::Undefined);
		std::cout<<(IsPossible(colors)?"possible":"impossible");
		const auto finish=std::chrono::steady_clock::now();
		const auto ms=
			std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
		std::cout<<", "<<ms/1000.<<"s"<<std::endl;
	}
	return 0;
}
