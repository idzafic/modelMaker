#include <gui/plot/Function.h>

namespace gui
{
namespace plot
{

gui::Point Function::findIntersection(const gui::Point& p1, const gui::Point& p2, const gui::Rect& r) // q1 i q2 su ravni rect sa tackama koji prikazuju dole, desno
{

	auto linesIntersect = [](const gui::Point& p1, const gui::Point& p2, const gui::Point& q1, const gui::Point& q2) -> gui::Point {

		bool verticalSide = (q1.y == q2.y) ? false : true;

		if (p1.y == p2.y) {
            if (!verticalSide)
			if (!verticalSide)
            {
                if (p1.y == q1.y)
                {
                    if (p1.x < p2.x)
                        return { q2.x, p1.y };
                    else
                        return { q1.x, p1.y };
                }
                else
                    return { q1.x - 1, q1.y - 1 };
            }
                

			return { q1.x, p1.y };

		}

		if (p1.x == p2.x) {
			if (verticalSide)
            {
                if (p1.x == q1.x)
                {
                    if (p1.y < p2.y)
                        return { p1.x, q2.y };
                    else
                        return { p1.x, q1.y };
                }
                else
                    return { q1.x - 1, q1.y - 1 };
            }

			return { p1.x, q1.y };
		}

		gui::CoordType p11 = p1.y * -1;
		gui::CoordType p22 = p2.y * -1;

		const gui::CoordType m = (p1.y - p2.y) / (p1.x - p2.x);
		const gui::CoordType c = p1.y - p1.x * m;
		if (verticalSide)
			return { q1.x, m * q1.x + c };
		return { (q1.y - c) / m, q1.y };
		};


	gui::Point p;

	const gui::Point& outsidePoint = r.contains(p1) ? p2 : p1;


	if (outsidePoint.y <= r.top)
    {
		p = linesIntersect(p1, p2, { r.left, r.top }, { r.right, r.top });
		if (p.x >= r.left && p.x <= r.right)
			return p;
	}
	else if(outsidePoint.y >= r.bottom)
    {
		p = linesIntersect(p1, p2, { r.left, r.bottom }, { r.right, r.bottom });
		if (p.x >= r.left && p.x <= r.right)
			return p;
	}


	if(outsidePoint.x >= r.right)
		return linesIntersect(p1, p2, { r.right, r.top }, { r.right, r.bottom });
	else
		return linesIntersect(p1, p2, { r.left, r.top }, { r.left, r.bottom });
	


}

Function::Function(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, const td::String& name, double lineWidth, Pattern pattern): color(color), _pattern(pattern), length(length), debljina(lineWidth)
{
	lines = new std::deque<gui::Shape>();
	this->name = new td::String(name);
	setPoints(x, y, length);
}


Function::Function(gui::Point* points, size_t length, td::ColorID color, const td::String& name, double lineWidth, Pattern pattern) : color(color), _pattern(pattern), length(length), debljina(lineWidth)
{
	lines = new std::deque<gui::Shape>();
	this->name = new td::String(name);
	tacke = new gui::Point[length];
	memcpy(tacke, points, sizeof(gui::Point)*length);
	memcpy(_tackeUnmodified, points, sizeof(gui::Point)*length);
}


Function::Function(Function&& f) noexcept
{
	*this = std::move(f);
}

Function& Function::operator=(Function&& f) noexcept
{
	memcpy(this, &f, sizeof(Function));
    f.name = nullptr;
	f.tacke = nullptr;
	f._tackeUnmodified = nullptr;
	f.lines = nullptr;

	return *this;

}

void Function::getScale(gui::CoordType& scaleXX, gui::CoordType& scaleYY) const{
	scaleXX = this->scaleX;
	scaleYY = this->scaleY;
}

void Function::getShift(gui::CoordType& shiftX, gui::CoordType& shiftY) const{
	shiftX = this->shiftX;
	shiftY = this->shiftY;
}


void Function::setPoints(gui::CoordType* x, gui::CoordType* y, size_t length){
	delete[] tacke;
	delete[] _tackeUnmodified;
	tacke = new gui::Point[length];
	_tackeUnmodified = new gui::Point[length];

	for (size_t i = 0; i < length; ++i){
		tacke[i].x = x[i];
		tacke[i].y = y[i];
		_tackeUnmodified[i].x = x[i];
		_tackeUnmodified[i].y = y[i];
	}

}

void Function::setPattern(Pattern pattern){
	this->_pattern = pattern;
	reDraw = true;
}

void Function::setLineWidth(double width){
	debljina = width;
	reDraw = true;
}


void Function::increaseScaleAndShiftX(const gui::CoordType& scale, const gui::CoordType& shift){
	scaleX *= scale;
	shiftX += shift;

	for (size_t i = 0; i < length; ++i)
		tacke[i].x = _tackeUnmodified[i].x * scaleX + shiftX;

	reDraw = true;
	
}

void Function::increaseScaleAndShiftY(const gui::CoordType& scale, const gui::CoordType& shift){
	scaleY *= scale;
	shiftY += shift;

	for (size_t i = 0; i < length; ++i)
		tacke[i].y = _tackeUnmodified[i].y * scaleY - shiftY;

	reDraw = true;
}

void Function::increaseShiftX(const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].x += shift;

	shiftX += shift;
	reDraw = true;
}

void Function::increaseShiftY(const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].y -= shift;

	shiftY += shift;
	reDraw = true;
}

void Function::increaseScaleX(const gui::CoordType& scale){
	scaleX *= scale;
	for (size_t i = 0; i < length; ++i)
		tacke[i].x = _tackeUnmodified[i].x * scaleX + shiftX;
	
	reDraw = true;
}

void Function::increaseScaleY(const gui::CoordType& scale) {
	scaleY *= scale;
	for (size_t i = 0; i < length; ++i)
		tacke[i].y = _tackeUnmodified[i].y * scaleY - shiftY;
	
	reDraw = true;
}

void Function::addToLines(std::vector<gui::Point>& tacke){
	if (!tacke.empty()) {
		lines->emplace_back();
		lines->back().createPolyLine(tacke.data(), tacke.size(), debljina, (td::LinePattern)_pattern.pattern);
		tacke.clear();
	}
}


gui::CoordType Function::realToTransformedX(const gui::CoordType& cord) const
{
    return cord * scaleX + shiftX;
}

gui::CoordType Function::transformedToRealX(const gui::CoordType& cord) const
{
    return (cord - shiftX) / scaleX;
}

gui::CoordType Function::realToTransformedY(const gui::CoordType& cord) const
{
    return cord * scaleY - shiftY;
}

gui::CoordType Function::TrasformedToRealY(const gui::CoordType& cord) const
{
    return (cord + shiftY) / scaleY;
}

void Function::draw(const gui::Rect& frame){
	if (length == 0)
		return;

	if (!reDraw) {
		for (auto& lin : *lines)
			lin.drawWire(color);
		return;
	}

	lines->clear();

	if ((int)_pattern.pattern > (int)Pattern::LinePattern::DefaultLine) {
		const td::DotPattern pattern = (td::DotPattern)_pattern.pattern;
		gui::Shape dot;
		std::vector<gui::Circle> dots;
		dots.reserve(length);
		for (size_t i = 0; i < length; ++i)
			if (frame.contains(tacke[i]))
				dots.emplace_back(tacke[i], 1);

		if(pattern == td::DotPattern::FilledCircle){
			dot.createCircles(dots.data(), dots.size(), debljina);
			dot.drawFillAndWire(color, color);
		}else{
			dot.createCircles(dots.data(), dots.size(), debljina);
			dot.drawWire(color);
		}


		return;
	}

	if (frame.isZero()) {
		lines->emplace_back();
		lines->back().createPolyLine(tacke, length, debljina, (td::LinePattern)_pattern.pattern);
		reDraw = false;
		draw(frame);
		return;
	}

	std::vector<gui::Point> lineParts;
	
	bool past = false;
	if (frame.contains(tacke[0])) {
		past = true;
		lineParts.emplace_back(tacke[0]);
	}


	for (size_t i = 1; i < length; ++i) {
		if (frame.contains(tacke[i])) {
			if (past)
				lineParts.emplace_back(tacke[i]);
			else {
				lineParts.emplace_back(findIntersection(tacke[i - 1], tacke[i], frame));
				lineParts.emplace_back(tacke[i]);
			}

			past = true;
			continue;
		}

		if (!past) { // ako dvije tacke zaredom nisu unutar prozora provjeri da li se njihov spoj sudara sa prozorom
			
			gui::Point p1 = findIntersection(tacke[i - 1], tacke[i], frame);
			if ((p1.x >= frame.left && p1.x <= frame.right) && (p1.y <= frame.bottom && p1.y >= frame.top)) {
				gui::Point p2 = findIntersection(tacke[i], tacke[i - 1], frame);
				if (p1.x != p2.x && p1.y != p2.y) {
					lineParts.emplace_back(p1);
					lineParts.emplace_back(findIntersection(tacke[i], tacke[i - 1], frame));
				}
			}
			continue;
		}
		past = false;

		lineParts.emplace_back(findIntersection(tacke[i - 1], tacke[i], frame));
		addToLines(lineParts);
	}

	addToLines(lineParts);

	reDraw = false;
	draw(frame);
}

Function::~Function()
{
    delete[] tacke;
    delete[] _tackeUnmodified;
    delete name;
    delete lines;
}

} //namepsace plot
} //namespace gui


