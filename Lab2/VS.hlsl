struct VS_INPUT
{
    float3 position : POSITION; // координаты вершины
    float4 color : COLOR; // цвет вершины (опционально)
};

// ----- Выходные данные из вершинного шейдера -----
// (Будут «входом» для пиксельного шейдера)
struct PS_INPUT
{
    float4 position : SV_Position; // позиция для растеризатора
    float4 color : COLOR; // цвет (или можно TEXCOORD0)
};


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    // Координаты в пространстве (здесь всё упрощённо)
    output.position = float4(input.position, 1.0);

    // Пробросим цвет из вершины дальше
    output.color = input.color;

    return output;
}