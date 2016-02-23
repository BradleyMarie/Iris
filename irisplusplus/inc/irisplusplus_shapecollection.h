/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_shapecollection.h

Abstract:

    This file contains the definitions for the Iris++ 
    ShapeCollection type used for building scenes.

--*/

#include <irisplusplus.h>
#include <vector>

#ifndef _SHAPE_COLLECTION_IRIS_PLUS_PLUS_
#define _SHAPE_COLLECTION_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class ShapeCollection final {
public:
    ShapeCollection(
        void
        )
    { }

    void
    Clear(
        void
        )
    {
        Shapes.clear();
    }
    
    void
    Add(
        _In_ Shape ToAdd,
        _In_ Matrix ModelToWorld = Matrix::Identity(),
        _In_ bool Premultiplied = false
        )
    {
        Shapes.push_back(std::make_tuple(ToAdd, ModelToWorld, Premultiplied));
    }
    
    ShapeCollection &
    operator=(
        _In_ const ShapeCollection & Collection
        ) = delete;
    
    ShapeCollection(
        _In_ const ShapeCollection & Collection
        ) = delete;
    
    typedef std::vector<std::tuple<Shape, Matrix, bool>>::const_iterator const_iterator;

    const_iterator
    begin(
        void
        ) const
    {
        return Shapes.begin();
    }

    const_iterator
    end(
        void
        ) const
    {
        return Shapes.end();
    }
    
private:
    std::vector<std::tuple<Shape, Matrix, bool>> Shapes;
};

} // namespace Iris

#endif // _SHAPE_COLLECTION_IRIS_PLUS_PLUS_