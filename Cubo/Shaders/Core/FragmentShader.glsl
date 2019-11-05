		#version 410 core\n
		in vec3 ourPos;
		in vec2 TexCoord;
		in vec3 ourNormal;
		uniform bool selecionado;
		out vec4 FragColor;
		uniform sampler2D texture1;
		void main()	{
		if (selecionado) {
		   FragColor = texture(texture1, TexCoord) * vec4(0.5,0.2,0.5,1.0);
		}else{
		   FragColor = texture(texture1, TexCoord) * vec4(1.0,1.0,1.0,1.0);
		}
		}