import Style from "@/styles/MenuTree.css";

export default function MenuTree(props) {
	const path = props.path ?? ""
	const data = props.data[props.fields[0]]

	if (props.fields.length === 1 || data.length === 0) return (
		<ul className="menuTree">
			{props.data[props.fields[0]].map((x, i) => {
				return (
					<li key={i}>
						{props.name(x.name, `${path}${x.name}`)}
					</li>
				)
			})}
		</ul>
	)
	else return (
		<ul className="menuTree">
			{data.map((x, i) => {
				return (
					<li key={i}>
						{props.name(x.name, `${path}${x.name}`)}
						<MenuTree
							data={x}
							fields={props.fields.slice(1)}
							name={props.name}
							path={`${path}${x.name}/`}
						/>
					</li>
				)
			})}
		</ul>
	)
}
