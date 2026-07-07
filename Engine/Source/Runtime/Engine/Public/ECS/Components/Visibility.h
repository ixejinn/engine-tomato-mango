#ifndef MANGO_VISIBILITY_H
#define MANGO_VISIBILITY_H

namespace tomato
{
	struct VisibilityComponent
	{
		bool visible{ true }; //self visible

		//Cache : Fanal parent's visible
		bool inheritedVisible{ true }; 
	};
}

#endif // !MANGO_VISIBILITY_H
