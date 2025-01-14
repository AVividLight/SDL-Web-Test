#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP


namespace Interface {
	namespace Internal {
		struct UIElements;
	}

	class UIElement {
	public:
		static UIElement Create();

	private:
		UIElement(Internal::UIElements& data, const int index);

		Internal::UIElements& Data;
		const int Index;
	};
}

#endif
