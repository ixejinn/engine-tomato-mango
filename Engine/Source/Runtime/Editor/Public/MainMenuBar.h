#ifndef MANGO_MAINMENUBAR_H
#define MANGO_MAINMENUBAR_H

namespace tomato
{
	struct EditorContext;

	class MainMenuBar
	{
	public:
		void Draw(EditorContext&);

	private:
		void MenuFile(EditorContext&);

		void SaveAs(EditorContext&);
	};
}
#endif // !MANGO_MAINMENUBAR_H
