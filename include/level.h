typedef struct
{
    int width;
    int height;
    int treeCount;
    Vector2 *trees;
} Level;

Level *AllocateLevel(int width, int height, int treeCount);
/**
 * Assumes level already has width, height and treeCount set
 */
void InitializeLevel(Level *level);
void DrawLevel(Level *level);