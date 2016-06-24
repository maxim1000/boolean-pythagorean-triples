#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
enum class TColor
{
	Black,
	White,
	Undefined
};
struct TTriplet
{
	int Elements[3];
};
using TTripletMap=std::vector<std::vector<TTriplet>>;//for quick getting all triplets for an element
std::vector<TTriplet> FindTriplets(int max)
{
	std::vector<int> roots(2*max*max+1,-1);
	for(int c=0;c*c<int(roots.size());++c)
		roots[c*c]=c;
	std::vector<TTriplet> triplets;
	for(int a=1;a<=max;++a)
	{
		for(int b=a+1;b<=max;++b)
		{
			const int c=roots[a*a+b*b];
			if(c!=-1 && c<=max)
				triplets.push_back({a,b,c});
		}
	}
	return triplets;
}
bool IsCorrect(
	const std::vector<TColor> &colors,
	const std::vector<TTriplet> &triplets)
{
	for(const auto &triplet:triplets)
	{
		bool hasWhite=false;
		bool hasBlack=false;
		bool hasUndefined=false;
		for(auto element:triplet.Elements)
		{
			if(colors[element]==TColor::Black)
				hasBlack=true;
			if(colors[element]==TColor::White)
				hasWhite=true;
			if(colors[element]==TColor::Undefined)
				hasUndefined=true;
		}
		if(!hasUndefined)
		{
			if(!hasBlack||!hasWhite)
				return false;
		}
	}
	return true;
}
bool IsCorrect(
	const std::vector<TColor> &colors,
	const TTripletMap &tripletMap,
	const std::vector<int> &elementSubset)
{
	for(int element:elementSubset)
	{
		if(!IsCorrect(colors,tripletMap[element]))
			return false;
	}
	return true;
}
TColor GetOppositeColor(TColor color)
{
	switch(color)
	{
		case TColor::Black:
			return TColor::White;
		case TColor::White:
			return TColor::Black;
		case TColor::Undefined:
			return TColor::Undefined;
		default:
			throw std::logic_error("unexpected value of TColor");
	}
}
std::vector<int> UpdateElements(
	std::vector<TColor> &colors,
	const TTripletMap &tripletMap,
	int notUpdatedElement)
{
	std::vector<int> updatedElements;
	updatedElements.push_back(notUpdatedElement);
	for(int index=0;index<int(updatedElements.size());++index)
	{
		for(const auto &triplet:tripletMap[updatedElements[index]])
		{
			for(int undefined=1;undefined<3;++undefined)
			{
				const int defined=1+2-undefined;
				if(colors[triplet.Elements[0]]!=colors[triplet.Elements[defined]])
					continue;
				if(colors[triplet.Elements[undefined]]!=TColor::Undefined)
					continue;
				colors[triplet.Elements[undefined]]=
					GetOppositeColor(colors[triplet.Elements[defined]]);
				updatedElements.push_back(triplet.Elements[undefined]);
			}
		}
	}
	updatedElements.erase(updatedElements.begin());
	return updatedElements;
}
bool IsColoringPossible(
	std::vector<TColor> &colors,
	const TTripletMap &tripletMap,
	int element,
	TColor newColor)
{
	colors[element]=newColor;
	auto updatedElements=UpdateElements(colors,tripletMap,element);
	updatedElements.push_back(element);
	const bool correct=IsCorrect(colors,tripletMap,updatedElements);
	for(int e:updatedElements)
		colors[e]=TColor::Undefined;
	return correct;
}
int SelectElementWithQuickCutoff(
	std::vector<TColor> &colors,
	const TTripletMap &tripletMap)
{
	for(int element=0;element<int(tripletMap.size());++element)
	{
		if(colors[element]!=TColor::Undefined)
			continue;
		for(auto newColor:{TColor::Black,TColor::White})
		{
			if(!IsColoringPossible(colors,tripletMap,element,newColor))
				return element;
		}
	}
	return -1;
}
int SelectNextElementToGuess(
	std::vector<TColor> &colors,
	const TTripletMap &tripletMap)
{
	int nextElement=SelectElementWithQuickCutoff(colors,tripletMap);
	if(nextElement!=-1)
		return nextElement;
	for(int element=0;element<int(tripletMap.size());++element)
	{
		if(colors[element]!=TColor::Undefined)
			continue;
		if(tripletMap[element].empty())
			continue;
		if(nextElement==-1 || tripletMap[element].size()>tripletMap[nextElement].size())
			nextElement=element;
	}
	return nextElement;
}
bool IsPossible(
	std::vector<TColor> &colors,
	const std::vector<TTriplet> &triplets,
	const TTripletMap &tripletMap)
{
	const int nextElement=SelectNextElementToGuess(colors,tripletMap);
	if(nextElement==-1)
		return true;
	for(auto newColor:{TColor::Black,TColor::White})
	{
		colors[nextElement]=newColor;
		auto updatedElements=UpdateElements(colors,tripletMap,nextElement);
		updatedElements.push_back(nextElement);
		if(IsCorrect(colors,tripletMap,updatedElements))
		{
			if(IsPossible(colors,triplets,tripletMap))
				return true;
		}
		for(int element:updatedElements)
			colors[element]=TColor::Undefined;
	}
	return false;
}
//When an element has only one triplet, we will always be able to select
//a value for it. So these elements limit nothing and their triplets can
//be processed later. If after extracting some triplet another one becomes
//redundant it will be extracted too.
std::vector<TTriplet> ExtractRedundantTriplets(
	std::vector<TTriplet> &triplets,
	int max)
{
	std::vector<TTriplet> redundant;
	while(true)
	{
		std::vector<int> popularity(max+1,0);
		for(const auto &triplet:triplets)
			for(int element:triplet.Elements)
				++popularity[element];
		int elementWithSingleTriplet=-1;
		for(int element=0;element<=max;++element)
			if(popularity[element]==1)
				elementWithSingleTriplet=element;
		if(elementWithSingleTriplet==-1)
			break;
		const auto predicate=[&](const TTriplet &triplet)
		{
			const auto count=std::count(
				std::begin(triplet.Elements),
				std::end(triplet.Elements),
				elementWithSingleTriplet);
			return count==0;
		};
		const auto redundantStart=
			std::partition(triplets.begin(),triplets.end(),predicate);
		redundant.insert(redundant.end(),redundantStart,triplets.end());
		triplets.erase(redundantStart,triplets.end());
	}
	return redundant;
}
//After we process all elements with triplets, the rest can be colored in any
//way.
void ColorUndefinedSomehow(std::vector<TColor> &colors)
{
	for(auto &color:colors)
		if(color==TColor::Undefined)
			color=TColor::Black;
}
TTripletMap BuildTripletMap(const std::vector<TTriplet> &triplets,int max)
{
	TTripletMap tripletMap(max+1);
	for(const auto &triplet:triplets)
	{
		for(int element:triplet.Elements)
		{
			auto ordered=triplet;
			while(ordered.Elements[0]!=element)
			{
				std::swap(ordered.Elements[0],ordered.Elements[1]);
				std::swap(ordered.Elements[1],ordered.Elements[2]);
			}
			tripletMap[element].push_back(ordered);
		}
	}
	return tripletMap;
}
bool IsPossible(int max)
{
	std::vector<TColor> colors(max+1,TColor::Undefined);
	auto triplets=FindTriplets(max);
	const auto redundant=ExtractRedundantTriplets(triplets,max);
	if(!IsPossible(colors,triplets,BuildTripletMap(triplets,max)))
		return false;
	if(!IsPossible(colors,redundant,BuildTripletMap(redundant,max)))
		throw std::logic_error("could not color elements of redundant triplets");
	ColorUndefinedSomehow(colors);
	if(!IsCorrect(colors,FindTriplets(max)))
		throw std::logic_error("found an incorrect coloring");
	return true;
}
int main()
{
	try
	{
		for(int max=4360;max<8000;++max)
		{
			const auto start=std::chrono::steady_clock::now();
			std::cout<<max<<": ";
			std::cout<<(IsPossible(max)?"possible":"impossible");
			const auto finish=std::chrono::steady_clock::now();
			const auto ms=
				std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
			std::cout<<", "<<ms/1000.<<"s"<<std::endl;
		}
	}
	catch(const std::exception &e)
	{
		std::cerr<<e.what()<<std::endl;
		return 1;
	}
	return 0;
}
