import Style from "@/styles/MenuTree.css";

export default function MenuTree({ path, data, fields, name }) {
	const targetData = data[fields[0]]

	if (fields.length === 1 || targetData.length === 0) return (
		<ul className="menuTree">
			{data[fields[0]].map((x, i) => {
				return (
					<li key={i}>
						{name(x.name, `${path ?? ""}${x.name}`)}
					</li>
				)
			})}
		</ul>
	)
	else return (
		<ul className="menuTree">
			{targetData.map((x, i) => {
				return (
					<li key={i}>
						{name(x.name, `${path}${x.name}`)}
						<MenuTree
							data={x}
							fields={fields.slice(1)}
							name={name}
							path={`${path ?? ""}${x.name}/`}
						/>
					</li>
				)
			})}
		</ul>
	)
}
